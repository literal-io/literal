package io.literal.lib;

import android.os.Build;

import androidx.annotation.RequiresApi;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class Crypto {
    public static String sha256Hex(String input) throws NoSuchAlgorithmException {
        MessageDigest digest = MessageDigest.getInstance("SHA-256");

        byte[] hash = null;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            hash = digest.digest(input.getBytes(StandardCharsets.UTF_8));
        } else {
            hash = digest.digest(input.getBytes());
        }

        StringBuffer output = new StringBuffer();
        for (int i = 0; i < hash.length; i++) {
            String hex = Integer.toHexString(0xff & hash[i]);
            if(hex.length() == 1) output.append('0');
            output.append(hex);
        }
        return output.toString();
    }
}
