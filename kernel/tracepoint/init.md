# tracer init流程
## 代码流程

```C/C++
//============FILE:trace/trace.c=============
fs_initcall(tracer_init_tracefs);       //初始化函数定义

static __init int tracer_init_tracefs(void)                      
{                                                                
    struct dentry *d_tracer;                                     
                                                                 
    trace_access_lock_init();                                    
                                                                 
	/* 
	 * 主要是初始化/sys/debug/tracing 这个dentry, 在新版内核中，
	 * 该tracing是一个mountpoint，fs type为tracefs

    d_tracer = tracing_init_dentry();	
    if (IS_ERR(d_tracer))                                        
        return 0;                                                
                                                                 
    event_trace_init();                                          
                                                                 
    init_tracer_tracefs(&global_trace, d_tracer);                
    ftrace_init_tracefs_toplevel(&global_trace, d_tracer);       
                                                                 
    trace_create_file("tracing_thresh", 0644, d_tracer,          
            &global_trace, &tracing_thresh_fops);                
                                                                 
    trace_create_file("README", 0444, d_tracer,                  
            NULL, &tracing_readme_fops);                         
                                                                 
    trace_create_file("saved_cmdlines", 0444, d_tracer,          
            NULL, &tracing_saved_cmdlines_fops);                 
                                                                 
    trace_create_file("saved_cmdlines_size", 0644, d_tracer,     
              NULL, &tracing_saved_cmdlines_size_fops);          
                                                                 
    trace_create_file("saved_tgids", 0444, d_tracer,             
            NULL, &tracing_saved_tgids_fops);                    
                                                                 
    trace_eval_init();                                           
                                                                 
    trace_create_eval_file(d_tracer);                            
                                                                 
#ifdef CONFIG_MODULES                                            
    register_module_notifier(&trace_module_nb);                  
#endif                                                           
                                                                 
#ifdef CONFIG_DYNAMIC_FTRACE                                     
    trace_create_file("dyn_ftrace_total_info", 0444, d_tracer,   
            &ftrace_update_tot_cnt, &tracing_dyn_info_fops);     
#endif                                                           
                                                                 
    create_trace_instances(d_tracer);                            
                                                                 
    update_tracer_options(&global_trace);                        
                                                                 
    return 0;                                                    
}                                                                

```
