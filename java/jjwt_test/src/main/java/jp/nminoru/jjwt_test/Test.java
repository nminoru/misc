package jp.nminoru.jjwt_test;

import java.security.KeyPair;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.PublicKey;
import io.jsonwebtoken.Claims;
import io.jsonwebtoken.Jws;
import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.SignatureAlgorithm;
import io.jsonwebtoken.security.Keys;

public class Test {
    
    public static void main(String[] args) {
        // 公開秘密鍵の生成
        KeyPair keyPair = Keys.keyPairFor(SignatureAlgorithm.RS256);
        PublicKey publicKey = (PublicKey)keyPair.getPublic();
        PrivateKey privateKey = (PrivateKey)keyPair.getPrivate();

        String jwsString =
            Jwts.builder()
            .setSubject("abc")
            .signWith(privateKey)
            .compact();

        System.out.println("JWS: " + jwsString);

        Jws<Claims> parsedJws =
            Jwts.parser()
            .setSigningKey(publicKey)
            .parseClaimsJws(jwsString);

        System.out.println("sub: " + parsedJws.getBody().getSubject());

    }
}
