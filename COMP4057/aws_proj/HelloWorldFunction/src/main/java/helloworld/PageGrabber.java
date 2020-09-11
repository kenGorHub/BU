package helloworld;

import java.net.*;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.io.*;
    public class PageGrabber {
        public static String loadWebPage(String urlString) {
            byte[] buffer = new byte[80*1024];
            String content = new String();
            try {
                URL url = new URL(urlString);
                URLConnection connection = url.openConnection();
                connection.setRequestProperty("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.95 Safari/537.11");
                connection.connect();
                BufferedReader r = new BufferedReader(new
                        InputStreamReader(connection.getInputStream(), Charset.forName("UTF-8")));
                String line;
                while ((line = r.readLine()) != null) {
                    content += line;
                }

                String data = content;
                int start = data.indexOf("<div class=\"product-price\">");
                System.out.println(start);
                int end = data.indexOf("<span class=\"product-prop\">", start);
                System.out.println(end);
                data = data.substring(start, end);
                System.out.println(data);
                String[] splited = data.split("<span");
                System.out.println(splited[0]);

            } catch (IOException e) {
                content = e.getMessage() + ": " + urlString;
            }
            return content;
        }

        public static void main(String[] args) throws Exception {
            String firstUrlAddress = "https://www.price.com.hk/product.php?p=366266&hw=h";
            System.out.println(loadWebPage(firstUrlAddress));
        }
}
