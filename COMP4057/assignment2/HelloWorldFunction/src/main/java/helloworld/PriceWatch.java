package helloworld;

import com.amazonaws.auth.AWSCredentials;
import com.amazonaws.auth.AWSStaticCredentialsProvider;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3ClientBuilder;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;


import java.io.*;
import java.net.URL;
import java.net.URLConnection;
import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.Base64;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class PriceWatch implements RequestHandler<Object, Object> {
    public Object handleRequest(final Object input, final Context context) {
        int status = 0;
        String firstUrlAddress = "https://www.price.com.hk/product.php?p=366266&hw=h";
        String serialized = null;
        HashMap<String, String> items = new HashMap<>();
        try {
            //get lowest price
            items.put("price", loadWebPage(firstUrlAddress));
            serialized = serialize(items);
        } catch (IOException e) {
            e.printStackTrace();
        }
        Regions region = Regions.US_EAST_1;
        AWSCredentials credentials = new BasicAWSCredentials(
                "AKIAT2YOH6JUCKMS37LU",
                "H8qx8ibv9lFPFqkHMmoqll+bU1TvVCVjAHD9zFx2"
        );
        String bucket = "19220456";
        String objKey = "pricewatch";

        // Serialize the hash map and write the data to S3
        AmazonS3 s3Client = AmazonS3ClientBuilder
                .standard()
                .withCredentials(new AWSStaticCredentialsProvider(credentials))
                .withRegion(region)
                .build();
        //upload price into bucket
        s3Client.putObject(bucket, objKey, serialized);
        status = 200;


        Map<String, String> headers = new HashMap<>();
        headers.put("Content-Type", "application/json");
        headers.put("X-Custom-Header", "application/json");

        String output = String.format("{ \"message\": \"PriceWatch\", \"Price\": \"%s\" }", items.toString());
        return new GatewayResponse(output, headers, 200);
    }
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

            //start here
            String data = content;
            Pattern pattern = Pattern.compile("(data-price=\")(.*?)(\")");
            Matcher matcher = pattern.matcher(data);
            double prices[]=new double[2];
            int count=0;
            while(matcher.find()&&count<2)
                prices[count++] = Double.parseDouble(matcher.group(2));
            Arrays.sort(prices);
            content = ""+prices[0];

        } catch (IOException e) {
            content = e.getMessage() + ": " + urlString;
        }
        return content;
    }

    private static String serialize(Serializable o) throws IOException {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        ObjectOutputStream oos = new ObjectOutputStream(baos);
        oos.writeObject(o);
        oos.close();
        return Base64.getEncoder().encodeToString(baos.toByteArray());
    }
}
