FROM gcr.io/cloud-devrel-public-resources/gcloud-container-1.14.0:latest

ADD ./model /tmp/mounted_model/0001
EXPOSE 8501

CMD tensorflow_model_server --port=8500 --rest_api_port=8501 --model_base_path=/tmp/mounted_model/ --tensorflow_session_parallelism=0 --file_system_poll_wait_seconds=31540000
