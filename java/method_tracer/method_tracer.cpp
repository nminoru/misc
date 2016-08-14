/*
 * Method Tracer
 *
 * Copyright (c) 2016 Minoru NAKAMURA <nminoru@nminoru.jp>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <jni.h>
#include <jvmti.h>

#define MAX_STACK_DEPTH  (256)

struct stack_info {
    char name[512];
};

struct thread_local_data {
    thread_local_data() : stack_level(0) {};

    int thread_id;
    int stack_level;

    stack_info stack[MAX_STACK_DEPTH];
};

static jvmtiEnv*        jvmti;
static int              global_thread_id;
static jrawMonitorID    thread_id_lock;
static pthread_key_t    tsd_key;

static void JNICALL vm_init(jvmtiEnv* jvmti, JNIEnv* jni_env, jthread thread);
static void JNICALL vm_death(jvmtiEnv* jvmti, JNIEnv* jni_env);
static void JNICALL vm_thread_start(jvmtiEnv* jvmti, JNIEnv* jni_env, jthread thread);
static void JNICALL vm_thread_end(jvmtiEnv* jvmti, JNIEnv* jni_env, jthread thread);
static void JNICALL vm_method_entry(jvmtiEnv* jvmti, JNIEnv* jni_env,  jthread thread, jmethodID method);
static void JNICALL vm_method_exit(jvmtiEnv* jvmti, JNIEnv* jni_env, jthread thread, jmethodID method, jboolean was_popped_by_exception, jvalue return_value);
static size_t parseJavaSignature(char *dest, const size_t limit, const char *src);
static size_t _parseJavaSignature(char *dest, const size_t limit, const char **src_p);


JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *vm, char *options, void *reserved) 
{
    jint                rc;
    jvmtiError          err;
    jvmtiCapabilities   capabilities;
    jvmtiEventCallbacks callbacks;

    /* Get JVMTI environment */
    rc = vm->GetEnv((void **)&jvmti, JVMTI_VERSION);
    if (rc != JNI_OK) {
        fprintf(stderr, "ERROR: Unable to create jvmtiEnv, GetEnv failed, error=%d\n", rc);
        return -1;
    }

    /* Get/Add JVMTI capabilities */ 
    err = jvmti->GetCapabilities(&capabilities);
    if (err != JVMTI_ERROR_NONE) {
        fprintf(stderr, "ERROR: GetCapabilities() failed, error=%d\n", err);
        return -1;
    }
    
    capabilities.can_generate_method_entry_events = 1;
    capabilities.can_generate_method_exit_events = 1;
    
    err = jvmti->AddCapabilities(&capabilities);
    if (err != JVMTI_ERROR_NONE) {
        fprintf(stderr, "ERROR: AddCapabilities() failed, error=%d\n", err);
        return -1;
    }

    /* Set all callbacks and enable VM_INIT event notification */
    memset(&callbacks, 0, sizeof(callbacks));

    callbacks.VMInit         = &vm_init;
    callbacks.VMDeath        = &vm_death;
    callbacks.ThreadStart    = &vm_thread_start;
    callbacks.ThreadEnd      = &vm_thread_end;
    callbacks.MethodEntry    = &vm_method_entry;
    callbacks.MethodExit     = &vm_method_exit;

    rc = jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
    if (err != JVMTI_ERROR_NONE) {
        fprintf(stderr, "ERROR: SetEventCallbacks() failed, error=%d\n", err);
        return -1;
    }

    rc = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, NULL);
    rc = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, NULL);
    rc = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_START, NULL);
    rc = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_END, NULL);
    rc = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY, NULL);
    rc = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_EXIT, NULL);

    jvmti->CreateRawMonitor("thread_id", &thread_id_lock);

    int ret;

    ret = pthread_key_create(&tsd_key, NULL);
    if (ret != 0) {
        fprintf(stderr, "ERROR: pthread_key_create() failed, error=%d\n", ret);
        return -1;
    }

    return 0;
}

static void JNICALL
vm_init(jvmtiEnv *jvmti, JNIEnv *env, jthread thread) 
{
}
 
static void JNICALL
vm_death(jvmtiEnv *jvmti, JNIEnv *env)
{
}

static void JNICALL
vm_thread_start(jvmtiEnv* jvmti, JNIEnv* jni_env, jthread thread)
{
    int ret, thread_id;
    thread_local_data* tld;

    jvmti->RawMonitorEnter(thread_id_lock);
    thread_id = ++global_thread_id;
    jvmti->RawMonitorExit(thread_id_lock);

    tld = new thread_local_data();
    tld->thread_id = thread_id;

    printf("%s at %s:%u\n", __func__, __FILE__, __LINE__);

    ret = pthread_setspecific(tsd_key, tld);
    if (ret != 0) {
        fprintf(stderr, "ERROR: pthread_setspecific() failed, error=%d\n", ret);
        abort();
    }
}

static void JNICALL
vm_thread_end(jvmtiEnv* jvmti, JNIEnv* jni_env, jthread thread)
{
}

static void JNICALL
vm_method_entry(jvmtiEnv* jvmti, JNIEnv* jni_env, jthread thread, jmethodID method)
{
    jvmtiError          err;
    thread_local_data*  tld;
    jclass klass;
    char* class_signature;
    char* class_generic;
    char* method_name;
    char* method_signature;
    char* method_generic;

    tld = (thread_local_data *) pthread_getspecific(tsd_key);
    if (tld == NULL)
        return;

    err = jvmti->GetMethodDeclaringClass(method, &klass);
    if (err != JVMTI_ERROR_NONE) {
        fprintf(stderr, "ERROR: GetMethodDeclaringClass() failed, error=%d\n", err);
        abort();
    }

    err = jvmti->GetClassSignature(klass, &class_signature, &class_generic);
    if (err != JVMTI_ERROR_NONE) {
        fprintf(stderr, "ERROR: GetClassSignature() failed, error=%d\n", err);
        abort();
    }

    err = jvmti->GetMethodName(method, &method_name, &method_signature, &method_generic);
    if (err != JVMTI_ERROR_NONE) {
        fprintf(stderr, "ERROR: GetMethodName() failed, error=%d\n", err);
        abort();
    }

    if (tld->stack_level < MAX_STACK_DEPTH)
    {
        char *dest = tld->stack[tld->stack_level].name;
        size_t size = 0;
        const size_t limit = sizeof(stack_info);

        size += parseJavaSignature(dest, limit, class_signature);

        if (limit < size + 2)
            goto parse_done;

        dest[size] = '.';
        size++;

        size += snprintf(dest + size, limit - size, "%s", method_name);

        if (limit < size + 1)
            goto parse_done;

        size += parseJavaSignature(dest + size, limit - size, method_signature);

    parse_done:
        printf("thread-%03d: ENTER %3d %s\n", tld->thread_id, tld->stack_level + 1,
               tld->stack[tld->stack_level].name);
    }
    else
    {
        printf("thread-%03d: ENTER %3d\n", tld->thread_id, tld->stack_level + 1);
    }

    tld->stack_level++;

    jvmti->Deallocate((unsigned char*) class_signature);
    jvmti->Deallocate((unsigned char*) class_generic);
    jvmti->Deallocate((unsigned char*) method_name);
    jvmti->Deallocate((unsigned char*) method_signature);
    jvmti->Deallocate((unsigned char*) method_generic);
}

static void JNICALL
vm_method_exit(jvmtiEnv* jvmti, JNIEnv* jni_env, jthread thread, jmethodID method, jboolean was_popped_by_exception, jvalue return_value)
{
    thread_local_data* tld;

    tld = (thread_local_data *) pthread_getspecific(tsd_key);

    if (tld == NULL)
        return;

    if ((0 < tld->stack_level) && (tld->stack_level < MAX_STACK_DEPTH))
        printf("thread-%03d: EXIT  %3d %s\n", tld->thread_id, tld->stack_level, tld->stack[tld->stack_level - 1].name);
    else
        printf("thread-%03d: EXIT  %3d\n", tld->thread_id, tld->stack_level);

    tld->stack_level--;
}

static size_t
parseJavaSignature(char *dest, const size_t limit, const char *src)
{
    return _parseJavaSignature(dest, limit, &src);
}

static size_t
_parseJavaSignature(char *dest, const size_t limit, const char **src_p)
{
    const char* src;
    size_t size = 0;

    src = *src_p;

    if (limit <= 1)
        return 0;

    if (!src)
        goto done;

    switch (*src) {

    case 'Z':
        size = snprintf(dest, limit, "boolean");
        src++;
        break;

    case 'B':
        size = snprintf(dest, limit, "byte");
        src++;
        break;

    case 'C':
        size = snprintf(dest, limit, "char");
        src++;
        break;

    case 'S':
        size = snprintf(dest, limit, "short");
        src++;
        break;

    case 'I':
        size = snprintf(dest, limit, "int");
        src++;
        break;

    case 'J':
        size = snprintf(dest, limit, "long");
        src++;
        break;
        
    case 'F':
        size = snprintf(dest, limit, "float");
        src++;
        break;

    case 'D':
        size = snprintf(dest, limit, "double");
        src++;
        break;

    case 'L': {
        bool class_name_end = false;

        src++; // skip 'L';

        while (*src && !class_name_end) {
            if (limit < size + 1)
                goto done;

            switch (*src) {

            case '/':
                dest[size] = '.';
                size++;
                break;

            case ';':
                class_name_end = true;
                break;

            default:
                dest[size] = *src;
                size++;
                break;
            }

            src++;
        }
        break;
    }

    case '[':
        if (limit < size + 1)
            goto done;

        src++;
        size = _parseJavaSignature(dest, limit, &src);

        if (limit < size + 2)
            goto done;

        dest[size++] = '[';
        dest[size++] = ']';
        break;

    case '(': {
        bool some_arguments = false;

        dest[size++] = '(';
        src++;

        do {
            if (*src == ')') {
                if (limit < size + 2)
                    goto done;

                dest[size++] = ')';
                dest[size++] = ' ';
                src++;
                break;
            } else {
                if (some_arguments) {
                    if (limit < size + 2)
                        goto done;

                    dest[size++] = ',';
                    dest[size++] = ' ';
                }

                if (limit < size + 1)
                    goto done;

                size += _parseJavaSignature(dest + size, limit - size, &src);
                some_arguments = true;
            }
        } while (*src);

        if (limit < size + 1)
            goto done;

        size += _parseJavaSignature(dest + size, limit - size, &src);
        break;
    }

    default:
        break;
    }

done:
    dest[size] = '\0';
    *src_p = src;

    return size;
}

