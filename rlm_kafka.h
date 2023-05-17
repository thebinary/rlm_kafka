typedef struct rlm_kafka_producer_section {

  // GLOBAL
  char const *metadata_broker_list;
  char const *bootstrap_servers;
  uint32_t statistics_interval_ms;
  bool api_version_request;
  char const *security_protocol;
  char const *sasl_mechanisms;
  char const *sasl_mechanism;
  char const *sasl_username;
  char const *sasl_password;
  char const *transactional_id;
  bool enable_idempotence;
  uint32_t queue_buffering_max_messages;
  uint32_t queue_buffering_max_kbytes;
  uint32_t queue_buffering_max_ms;
  uint32_t linger_ms;
  uint32_t message_send_max_retries;
  uint32_t retries;
  uint32_t message_max_bytes;
  uint32_t receive_message_max_bytes;
  char const *debug;
  uint32_t connections_max_idle_ms;
  uint32_t reconnect_backoff_ms;
  uint32_t reconnect_backoff_max_ms;
  uint32_t api_version_fallback_ms;
  char const *broker_version_fallback;
  uint32_t transaction_timeout_ms;
  uint32_t retry_backoff_ms;
  uint32_t batch_num_messages;
  uint32_t batch_size;
  char const *builtin_features;
  char const *client_id;
  uint32_t message_copy_max_bytes;
  uint32_t max_in_flight_requests_per_connection;
  uint32_t max_in_flight;
  int32_t topic_metadata_refresh_interval_ms;
  uint32_t metadata_max_age_ms;
  uint32_t topic_metadata_refresh_fast_interval_ms;
  uint32_t topic_metadata_refresh_fast_cnt;
  bool topic_metadata_refresh_sparse;
  uint32_t topic_metadata_propagation_max_ms;
  char const *topic_blacklist;
  uint32_t socket_timeout_ms;
  uint32_t socket_blocking_max_ms;
  uint32_t socket_send_buffer_bytes;
  uint32_t socket_receive_buffer_bytes;
  bool socket_keepalive_enable;
  bool socket_nagle_disable;
  uint32_t socket_max_fails;
  uint32_t broker_address_ttl;
  char const *broker_address_family;
  uint32_t reconnect_backoff_jitter_ms;
  uint32_t enabled_events;
  uint32_t log_level;
  bool log_queue;
  bool log_thread_name;
  bool enable_random_seed;
  bool log_connection_close;
  uint32_t internal_termination_signal;
  uint32_t api_version_request_timeout_ms;
  char const *ssl_cipher_suites;
  char const *ssl_curves_list;
  char const *ssl_sigalgs_list;
  char const *ssl_key_location;
  char const *ssl_key_password;
  char const *ssl_key_pem;
  char const *ssl_certificate_location;
  char const *ssl_certificate_pem;
  char const *ssl_ca_location;
  char const *ssl_ca_certificate_stores;
  char const *ssl_crl_location;
  char const *ssl_keystore_location;
  char const *ssl_keystore_password;
  char const *ssl_engine_location;
  char const *ssl_engine_id;
  bool enable_ssl_certificate_verification;
  char const *ssl_endpoint_identification_algorithm;
  char const *sasl_kerberos_service_name;
  char const *sasl_kerberos_principal;
  char const *sasl_kerberos_kinit_cmd;
  char const *sasl_kerberos_keytab;
  uint32_t sasl_kerberos_min_time_before_relogin;
  char const *sasl_oauthbearer_config;
  bool enable_sasl_oauthbearer_unsecure_jwt;
  char const *plugin_library_paths;
  char const *client_rack;
  bool enable_gapless_guarantee;
  uint32_t queue_buffering_backpressure_threshold;
  bool delivery_report_only_error;
  uint32_t sticky_partitioning_linger_ms;
  
  // TOPIC
  int32_t request_required_acks;
  int32_t acks;
  uint32_t message_timeout_ms;
  uint32_t delivery_timeout_ms;
  char const *partitioner;
  char const *compression_codec;
  uint32_t request_timeout_ms;
  char const *compression_type;
  int32_t compression_level;
  char const *queuing_strategy;
  bool produce_offset_report;

} rlm_kafka_producer_section_t;

typedef struct rlm_kafka_acct_section {
  CONF_SECTION *ms;

  char const *reference;
  char const *start;
  char const *stop;
  char const *interim_update;
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
  rlm_kafka_producer_section_t producer;
} rlm_kafka_t;



#define RLM_KAFKA_PROP_DEBUG(PROP, VALUE) DEBUG3("rlm_kafka: Setting producer property '"PROP"' to '%s'\n", VALUE)
#define RLM_KAFKA_PROP_ERROR(PROP, ERR_STRING) ERROR("property="PROP", %s\n", ERR_STRING)
