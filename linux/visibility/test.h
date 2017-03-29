#ifndef TEST_H

extern void call_default_func1(void);
extern void call_default_func2(void);
extern void __attribute__((visibility ("default"))) default_func(void);

extern void call_protected_func1(void);
extern void call_protected_func2(void);
extern void __attribute__((visibility ("protected"))) protected_func(void);

extern void call_hidden_func1(void);
extern void call_hidden_func2(void);
extern void __attribute__((visibility ("hidden"))) hidden_func(void);

extern void call_internal_func1(void);
extern void call_internal_func2(void);
extern void __attribute__((visibility ("internal"))) internal_func(void);

extern int __attribute__((visibility ("default"))) default_var;
extern int __attribute__((visibility ("protected"))) protected_var;
extern int __attribute__((visibility ("hidden"))) hidden_var;

#endif /* TEST_H */
