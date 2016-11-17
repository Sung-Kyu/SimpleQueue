#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>

pthread_mutex_t  m_lock = PTHREAD_MUTEX_INITIALIZER;

constexpr int total_count = 15;
constexpr int buffer_data_size = 640*480;
constexpr int buffer_total_count = total_count + 1;

static uint8_t m_buffer[buffer_total_count][buffer_data_size];
static int m_front = 0;
static int m_rear = 0;
static int m_buffer_count = 0;

void push(uint8_t *data, int size)
{
    printf("Push >>>>  \n");

    pthread_mutex_lock(&m_lock);

    if (size < 1 || size > buffer_data_size)
        size = buffer_data_size;

    memset(m_buffer + m_rear, 0x00, size);
    memcpy(m_buffer + m_rear, data, size);    
    
    ++m_rear;

    if (m_rear >= buffer_total_count)
        m_rear = 0;

    if (m_rear == m_front) {        
        ++m_front;

        if (m_front >= buffer_total_count)
            m_front = 0;
    }

    if (m_buffer_count < (buffer_total_count - 1))
        ++m_buffer_count;

    // printf("front[%d] rear[%d] \n", m_front, m_rear);
    // printf("m_buffer_count[%d] \n", m_buffer_count);
    // printf("push value[%d] \n", data[0]);
    // printf("========================= \n");
    pthread_mutex_unlock(&m_lock);    
}

int pop(uint8_t *data, int size)
{
    printf("<<<< Pop ... \n");

    pthread_mutex_lock(&m_lock);

    if (m_rear == m_front) {
        pthread_mutex_unlock(&m_lock);
        return 0;
    }

    if (size < 1 || size > buffer_data_size)
        size = buffer_data_size;    

    memcpy(data, m_buffer + m_front, size);
    
    ++m_front;

    if (m_front >= buffer_total_count)
        m_front = 0;

    if (m_buffer_count > 0)
        --m_buffer_count;
    
    // printf("front[%d] rear[%d] \n", m_front, m_rear);
    // printf("m_buffer_count[%d] \n", m_buffer_count);
    printf("pop value[%d] \n", data[0]);
    // printf("========================= \n");
    pthread_mutex_unlock(&m_lock);

    return size;
}

int front(uint8_t *data, int size)
{
    printf("<<<< Front ... \n");

    pthread_mutex_lock(&m_lock);

    if (m_rear == m_front) {
        pthread_mutex_unlock(&m_lock);
        return 0;
    }

    if (size < 1 || size > buffer_data_size)
        size = buffer_data_size;    

    memcpy(data, m_buffer + m_front, size);       
    
    printf("front value[%d] \n", data[0]);

    pthread_mutex_unlock(&m_lock);

    return size;
}

int back(uint8_t *data, int size)
{
    printf("<<<< Back ... \n");

    pthread_mutex_lock(&m_lock);

    if (m_rear == m_front) {
        pthread_mutex_unlock(&m_lock);
        return 0;
    }

    if (size < 1 || size > buffer_data_size)
        size = buffer_data_size;    

    memcpy(data, m_buffer + m_rear, size);       
    
    printf("Back value[%d] \n", data[0]);

    pthread_mutex_unlock(&m_lock);

    return size;
}

int getCount()
{
    return m_buffer_count;
}

uint8_t image1[640*480] = {1,};

int main()
{        
    printf("<<< test ... >>> \n");
    constexpr int count = 20;
    
    for (int i = 0; i < count; ++i) {
        image1[0] = i;
        push(image1, buffer_data_size);
    }

    for (int i = 0; i < buffer_total_count; ++i) {
        printf("m_buffer[%d] \n", m_buffer[i][0]);
    }

    printf("Buffer Count[%d] \n\n", getCount());
    
    front(image1, buffer_data_size);
    back(image1, buffer_data_size);

    for (int i = 0; i < buffer_total_count; ++i) {
        pop(image1, buffer_data_size);        
    }

    printf("Buffer Count[%d] \n\n", getCount());

    int status = pthread_mutex_destroy(&m_lock); 
    printf("code = %d \n", status);

    return 0;
}