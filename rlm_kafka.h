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
  rd_kafka_topic_conf_t *tconf;

  rlm_kafka_acct_section_t accounting;
} rlm_kafka_t;

#define MDEBUG3(FMT, __VARGS__...) DEBUG3("rlm_kafka: "FMT, ## __VARGS__)
#define MDEBUG4(FMT, __VARGS__...) DEBUG4("rlm_kafka: "FMT, ## __VARGS__)
#define MERROR(FMT, __VARGS__...) ERROR("rlm_kafka: "FMT, ## __VARGS__)

#define RLM_KAFKA_PROP_DEBUG(PROP, VALUE) MDEBUG3("Setting producer property '%s=%s'\n", PROP, VALUE)
#define RLM_KAFKA_PROP_ERROR(PROP, VALUE, ERR_STRING) MERROR("Error setting properry: '%s=%s' : %s\n", PROP, VALUE, ERR_STRING)
#define RLM_KAFKA_PROP_SET(CONF, PROP, VALUE, BUF_ERRSTR) RLM_KAFKA_PROP_DEBUG(PROP, VALUE); if (rd_kafka_conf_set(CONF, PROP, VALUE, BUF_ERRSTR, sizeof(BUF_ERRSTR)) != RD_KAFKA_CONF_OK ) { RLM_KAFKA_PROP_ERROR(PROP, VALUE, BUF_ERRSTR); }

#define RLM_KAFKA_TOPIC_PROP_DEBUG(PROP, VALUE) MDEBUG3("Setting topic property '%s=%s'\n", PROP, VALUE)
#define RLM_KAFKA_TOPIC_PROP_ERROR(PROP, VALUE, ERR_STRING) MERROR("Error setting topic property: '%s=%s' : %s\n", PROP, VALUE, ERR_STRING)
#define RLM_KAFKA_TOPIC_PROP_SET(CONF, PROP, VALUE, BUF_ERRSTR) RLM_KAFKA_TOPIC_PROP_DEBUG(PROP, VALUE); if (rd_kafka_topic_conf_set(CONF, PROP, VALUE, BUF_ERRSTR, sizeof(BUF_ERRSTR)) != RD_KAFKA_CONF_OK ) { RLM_KAFKA_TOPIC_PROP_ERROR(PROP, VALUE, BUF_ERRSTR); }