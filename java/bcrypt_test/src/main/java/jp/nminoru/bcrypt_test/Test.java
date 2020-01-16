package jp.nminoru.bcrypt_test;

import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;


public class Test {
    
    public static void main(String[] args) {
        PasswordEncoder passwordEncoder = new BCryptPasswordEncoder();

        String password = "password";
        String digest = passwordEncoder.encode(password);
        
        System.out.println("hashcode = " + digest);

        if (passwordEncoder.matches(password, digest)) {
            System.out.println("match");
            return;
        }
        
        System.out.println("unmatch");
    }
}
