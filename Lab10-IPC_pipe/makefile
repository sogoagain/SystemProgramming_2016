all: myapp

myapp: ipc_producer ipc_consumer

ipc_producer: ipc_producer.c
	gcc -o ipc_producer ipc_producer.c

ipc_consumer: ipc_consumer.c
	gcc -o ipc_consumer ipc_consumer.c
