/*
 *   This program is is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or (at
 *   your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

/**
 * @file rlm_kafka.c
 * @brief Accounting to Kafka module
 *
 * @copyright 2021 TheBinary
 * @copyright 2021 binary4bytes@gmail.com
 */

#include <freeradius-devel/radiusd.h>
#include <freeradius-devel/modules.h>
#include <freeradius-devel/rad_assert.h>
#include <librdkafka/rdkafka.h>
#include "rlm_kafka.h"

#include <arpa/inet.h>

/*
Configuration example:

kafka {
    bootstrap-servers = "localhost:9092"
    topic = "radacct"
    producer-config {
         ...
    }
    accounting {
       reference = "<packet type xlat>"
       messages {
          start = "..."
          stop = "..."
	  interim-update = "..."
       }
    }
}
 */

static const CONF_PARSER messages_config[] = {
  { "start", FR_CONF_OFFSET(PW_TYPE_STRING | PW_TYPE_XLAT, rlm_kafka_t, accounting.start), NULL },
  { "stop", FR_CONF_OFFSET(PW_TYPE_STRING | PW_TYPE_XLAT, rlm_kafka_t, accounting.stop), NULL },
  { "interim-update", FR_CONF_OFFSET(PW_TYPE_STRING | PW_TYPE_XLAT, rlm_kafka_t, accounting.interim_update), NULL },

  CONF_PARSER_TERMINATOR
};

static const CONF_PARSER producer_config[] = {
  CONF_PARSER_TERMINATOR
};

static const CONF_PARSER acct_config[] = {
  { "reference", FR_CONF_OFFSET(PW_TYPE_STRING | PW_TYPE_XLAT, rlm_kafka_t, accounting.reference), NULL },
  { "messages", FR_CONF_POINTER(PW_TYPE_SUBSECTION, NULL), (void const*) messages_config },

  CONF_PARSER_TERMINATOR
};

static const CONF_PARSER stats_config[] = {
  {"file", FR_CONF_OFFSET(PW_TYPE_STRING, rlm_kafka_t, stats_filename), NULL},

  CONF_PARSER_TERMINATOR
};

static const CONF_PARSER module_config[] = {
  { "bootstrap-servers", FR_CONF_OFFSET(PW_TYPE_STRING | PW_TYPE_REQUIRED, rlm_kafka_t, bootstrap), NULL },
  { "topic", FR_CONF_OFFSET(PW_TYPE_STRING | PW_TYPE_REQUIRED, rlm_kafka_t, topic), NULL },
  { "producer-config", FR_CONF_POINTER(PW_TYPE_SUBSECTION, NULL), (void const*) producer_config },
  { "accounting", FR_CONF_POINTER(PW_TYPE_SUBSECTION, NULL), (void const*) acct_config },
  { "statistics", FR_CONF_POINTER(PW_TYPE_SUBSECTION, NULL), (void const*) stats_config },

  CONF_PARSER_TERMINATOR
};

static int stats_cb (rd_kafka_t *rk, char *json, size_t json_len,
		     void *opaque) {
  DEBUG3("rlm_kafka: stats callback");
  rlm_kafka_t *inst = opaque;
  FILE *fp = inst->stats_file;

  fprintf(fp, "%s\n", json);
  fflush(fp);
  return 0;
}

static int mod_instantiate(CONF_SECTION *conf, void *instance)
{
  rlm_kafka_t *inst = instance;
  char errstr[512];
  CONF_PAIR *cp = NULL;
  CONF_SECTION *cs = cf_section_sub_find(conf, "accounting");
  CONF_SECTION *pc = cf_section_sub_find(conf, "producer-config");
  
  inst->accounting.ms = cf_section_sub_find(cs, "messages");
  
  // Create Producer Configuration
  inst->kconf = rd_kafka_conf_new();
  
  // Set Producer Configration Properties
  if(rd_kafka_conf_set(inst->kconf, "bootstrap.servers", inst->bootstrap,
		       errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
    RLM_KAFKA_PROP_ERROR("bootstrap.servers", errstr);
  }
  
  if(inst->stats_filename) {
    DEBUG3("rlm_kafka: Setting instance as kafka opaque");
    rd_kafka_conf_set_opaque(inst->kconf, inst);

    DEBUG3("rlm_kafka: Opening statistics file for writing: %s", inst->stats_filename);
    inst->stats_file = fopen(inst->stats_filename, "a");
    if(inst->stats_file == NULL) {
      ERROR("rlm_kafka: error opening statistics file: %s", inst->stats_filename);
    }

    DEBUG3("rlm_kafka: Registering statistics callback");
    rd_kafka_conf_set_stats_cb(inst->kconf, stats_cb);
  }

  /* Search configuration items in producer conf_section
     and set the property of kafka producer */
  do {
    cp = cf_pair_find_next(pc, cp, NULL);
    if(cp) {
      char const *attr = cf_pair_attr(cp);
      char const *value = cf_pair_value(cp);
      DEBUG3("rlm_kafka: Setting producer property '%s' as '%s'\n", attr, value);
      if(rd_kafka_conf_set(inst->kconf, attr, value, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
	ERROR("rlm_kafka: property=%s, %s", attr, errstr);
      }
    }
  } while(cp != NULL);
  
  DEBUG3("rlm_kafka: Kafka config:");
  const char **arr;
  size_t cnt;
  arr = rd_kafka_conf_dump(inst->kconf, &cnt);
  for (int i = 0; i < (int)cnt; i += 2)
	  DEBUG3("rlm_kafka: \t%s = %s", arr[i], arr[i + 1]);

  // Create Producer
  inst->rk = rd_kafka_new(RD_KAFKA_PRODUCER, inst->kconf, errstr, sizeof(errstr));
  if (!inst->rk) {
    ERROR("rlm_kafka: Failed to create new producer: %s\n", errstr);
    return -1;
  }
  
  DEBUG3("rlm_kafka: Creating instance for topic: %s", inst->topic);
  inst->rkt = rd_kafka_topic_new(inst->rk, inst->topic, NULL);
  
  return RLM_MODULE_OK;
}

#ifdef WITH_ACCOUNTING

/*
 *	Write accounting information to Kafka
 */
static rlm_rcode_t CC_HINT(nonnull) mod_accounting(UNUSED void *instance, UNUSED REQUEST *request)
{
  rlm_kafka_t *inst = instance;
  rd_kafka_resp_err_t err;
  char message[4096];
  char ref[25];

  radius_xlat(ref, sizeof(ref) - 1, request, inst->accounting.reference, NULL, NULL);

  CONF_PAIR *cp = cf_pair_find(inst->accounting.ms, ref);
  const char *schema = cf_pair_value(cp);

  radius_xlat(message, sizeof(message) - 1, request, schema, NULL, NULL);
  
  size_t len = strlen(message);
  
  err = rd_kafka_producev(inst->rk,
          RD_KAFKA_V_RKT(inst->rkt),
          RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
          RD_KAFKA_V_VALUE(message, len),
          RD_KAFKA_V_OPAQUE(NULL),
          RD_KAFKA_V_END);
  if (err) {
    DEBUG3("rlm_kafka: Failed to produce to topic: %s: %s\n",
            inst->topic, rd_kafka_err2str(err));
  }

  /* non-blocking */
  DEBUG3("rlm_kafka: polling kafka");
  rd_kafka_poll(inst->rk, 0);

  return RLM_MODULE_OK;
}
#endif

/*
 *	Only free memory we allocated.  The strings allocated via
 *	cf_section_parse() do not need to be freed.
 */
static int mod_detach(UNUSED void *instance)
{
  rlm_kafka_t *inst = instance;

  if(inst->stats_file) {
    DEBUG3("rlm_kafka: Closing statistics file");
    fclose(inst->stats_file);
  }
  DEBUG3("rlm_kafka: Flushing kafka queue");
  rd_kafka_flush(inst->rk, 10*1000);
  DEBUG3("rlm_kafka: Releasing kafka topic");
  rd_kafka_topic_destroy(inst->rkt);
  DEBUG3("rlm_kafka: Stopping kafka producer");
  rd_kafka_destroy(inst->rk);
  return 0;
}

// Define the module
extern module_t rlm_kafka;
module_t rlm_kafka = {
	.magic		= RLM_MODULE_INIT,
	.name		= "kafka",
	.type		= RLM_TYPE_THREAD_SAFE,
	.inst_size	= sizeof(rlm_kafka_t),
	.config		= module_config,
	.instantiate	= mod_instantiate,
	.detach		= mod_detach,
	.methods = {
#ifdef WITH_ACCOUNTING
		[MOD_ACCOUNTING]	= mod_accounting
#endif
	},
};
