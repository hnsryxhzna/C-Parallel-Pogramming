#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_LEN 128


typedef struct {
    char content[MSG_LEN];
} message;

typedef struct {
    message messages[MSG_LEN];
    bool is_full;
    int position;
    pthread_mutex_t message_lock;
} message_box;

static message_box box0, box1;

int message_box_initialize(message_box* m) {
    m->is_full = false;
    m->position = 0;
    pthread_mutex_init(&m->message_lock, NULL);

    return 0;
}

int message_box_free(message_box* m) {
    pthread_mutex_destroy(&m->message_lock);

    // for (int i = 0; i < m->position; i++) {
    //     free(m->messages[i].content);
    // }

    return 0;
}

int message_send(message_box* m, message content) {
    pthread_mutex_lock(&m->message_lock);

    if (m->is_full) {
        printf("Message box is full\n");
        pthread_mutex_unlock(&m->message_lock);
        return 1;
    }

    m->messages[m->position] = content;
    m->position++;

    if (m->position == MSG_LEN) {
        m->is_full = true;
    }

    pthread_mutex_unlock(&m->message_lock);

    return 0;
}

int message_receive(message_box *m, message* message_buffer) {
    pthread_mutex_lock(&m->message_lock);

    if (m->position > 0) {
        strcpy(message_buffer->content, m->messages[m->position - 1].content);
    }

    pthread_mutex_unlock(&m->message_lock);

    return 0;
}

void* f0(void* arg) {
    (void) arg;
    char message_buffer[MSG_LEN];
    message new_message;
    message receive_message_buffer;

    while (
        printf("Person0 input: "),
        fgets(new_message.content, sizeof(new_message.content), stdin)
    ) {

        if (message_send(&box0, new_message) == 1) {
            printf("Message box if full\n");
            exit(-1);
        }

        message_receive(&box1, &receive_message_buffer);
        printf("Person0 got: %s\n", receive_message_buffer.content);
    }

    return NULL;   
}

void* f1(void* arg) {
    (void) arg;
    char message_buffer[MSG_LEN];
    message new_message;
    message receive_message_buffer;

    while (
        printf("Person1 input: "),
        fgets(new_message.content, sizeof(new_message.content), stdin)
    ) {

        if (message_send(&box1, new_message) == 1) {
            printf("Message box if full\n");
            exit(-1);
        }

        message_receive(&box0, &receive_message_buffer);
        printf("Person1 got: %s\n", receive_message_buffer.content);
    }

    return NULL;   
}

int main(void) {
    message_box_initialize(&box0);
    message_box_initialize(&box1);

    pthread_t p0, p1;

    if (pthread_create(&p0, NULL, f0, NULL) != 0|| pthread_create(&p1, NULL, f1, NULL) != 0) {
        printf("Error in creating thread\n");
        exit(-1);
    }

    pthread_join(p0, NULL);
    pthread_join(p1, NULL);

    message_box_free(&box0);
    message_box_free(&box1);

    return 0;
}
