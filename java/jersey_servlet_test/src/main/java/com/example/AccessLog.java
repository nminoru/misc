package com.example;

import lombok.extern.slf4j.Slf4j;

@Slf4j
public class AccessLog extends java.util.logging.Logger {
    public AccessLog() {
        super("Jersey", null);
    }

    @Override
    public void log(java.util.logging.Level level, String msg) {
        log.info(msg);            
    }
}    
