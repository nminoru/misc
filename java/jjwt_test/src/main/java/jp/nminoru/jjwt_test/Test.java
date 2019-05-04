package jp.nminoru.jjwt_test;

import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.X509EncodedKeySpec;
import java.util.Base64;
import io.jsonwebtoken.Claims;
import io.jsonwebtoken.Jws;
import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.SignatureAlgorithm;
import io.jsonwebtoken.security.Keys;

public class Test {
    
    public static void main(String[] args) throws InvalidKeySpecException, NoSuchAlgorithmException {
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

        Jws<Claims> parsedJws;
        parsedJws = Jwts.parser().setSigningKey(publicKey).parseClaimsJws(jwsString);

        System.out.println("sub: " + parsedJws.getBody().getSubject());
        
        // 公開鍵を BASE64 文字列に変更し、際
        String publicKeyEncoded = Base64.getEncoder().encodeToString(publicKey.getEncoded());
        System.out.println("public-key: " + publicKeyEncoded);

        byte[] publicKeyBytes = Base64.getDecoder().decode(publicKeyEncoded);
        X509EncodedKeySpec keySpec = new X509EncodedKeySpec(publicKeyBytes);
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");
        PublicKey publicKey2 = keyFactory.generatePublic(keySpec);

        parsedJws = Jwts.parser().setSigningKey(publicKey2).parseClaimsJws(jwsString);

        System.out.println("sub: " + parsedJws.getBody().getSubject());        
    }
}
