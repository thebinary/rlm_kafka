typedef struct rlm_kafka_acct_section {
  CONF_SECTION *messages;

  char const *reference;
  char const *key;
} rlm_kafka_acct_section_t;

/*
 *	Define a structure for our module configuration.
 *
 *	These variables do not need to be in a structure, but it's
 *	a lot cleaner to do so, and a pointer to the structure can
 *	be used as the instance handle.
 */
typedef struct rlm_kafka_t {
  char const *bootstrap;
  char const *topic;
  char const *schema;

  char const *stats_filename;
  FILE *stats_file; 

  rd_kafka_t *rk;
  rd_kafka_topic_t *rkt;
  rd_kafka_conf_t *kconf;

  rlm_kafka_acct_section_t accounting;
} rlm_kafka_t;



#define RLM_KAFKA_PROP_DEBUG(PROP, VALUE) DEBUG3("rlm_kafka: Setting producer property '"PROP"' to '%s'\n", VALUE)
#define RLM_KAFKA_PROP_ERROR(PROP, ERR_STRING) ERROR("property="PROP", %s\n", ERR_STRING)