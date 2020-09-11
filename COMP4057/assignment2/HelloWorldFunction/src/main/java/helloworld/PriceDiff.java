package helloworld;

import com.amazonaws.auth.AWSCredentials;
import com.amazonaws.auth.AWSStaticCredentialsProvider;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3ClientBuilder;
import com.amazonaws.services.s3.model.S3Object;
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
import java.util.stream.Collectors;

/**
 * Handler for requests to Lambda function.
 */
public class PriceDiff implements RequestHandler<Object, Object> {

    public Object handleRequest(final Object input, final Context context) {
        Map<String, String> headers = new HashMap<>();
        headers.put("Content-Type", "application/json");
        headers.put("X-Custom-Header", "application/json");
        try {
            //get web page
            final String page_Price = loadWebPage("https://www.price.com.hk/product.php?p=366266&hw=h");
            final String page_Boardway = loadWebPage("https://www.broadwaylifestyle.com/categories/usage/photo-supply-and-accessories/mirrorless-camera/z6-body-mount-adapter-ftz-mirrorless-a109708-m.html#general_colour=2143");

            String data = page_Price;
            Pattern pattern = Pattern.compile("(data-price=\")(.*?)(\")");
            Matcher matcher = pattern.matcher(data);
            double prices[]=new double[2];
            int count=0;
            while(matcher.find()&&count<2)
                prices[count++] = Double.parseDouble(matcher.group(2));
            Arrays.sort(prices);
            String price_P = ""+prices[0];

            Document doc = Jsoup.parse(page_Boardway);
            Elements tagetArea = doc.getElementsByClass("price-wrapper final-price-wrapper");

            String price_B = tagetArea.attr("data-price-amount");
            //find difference from two new price
            float new_deff = Math.abs(Float.parseFloat(price_P)-Float.parseFloat(price_B));

            //get old data
            String serialize = getBucketObject("pricewatch");
            String serialize2 = getBucketObject("BoardwayWatch");

            //deserialize bucket object
            String oldprice_P = deserialize(serialize);
            String oldprice_B = deserialize(serialize2);

            //find difference from two old price
            float old_deff = Math.abs(Float.parseFloat(oldprice_P)-Float.parseFloat(oldprice_B));

            //compare new data and old data
            String difference="";
            if(new_deff > old_deff)difference="increased";
            if(new_deff == old_deff)difference="unchanged";
            if(new_deff < old_deff)difference="decreased";


            String output = String.format("{ \"message\": \"PriceDiff\", \"Price difference\": \"%s\" }", difference);
            return new GatewayResponse(output, headers, 200);
        } catch (IOException | ClassNotFoundException e) {
            return new GatewayResponse("{}", headers, 500);
        }
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
        } catch (IOException e) {
            content = e.getMessage() + ": " + urlString;
        }
        return content;
    }

    private static String deserialize(String s) throws IOException, ClassNotFoundException {
        ByteArrayInputStream bais = new ByteArrayInputStream(Base64.getDecoder().decode(s));
        ObjectInputStream ois = new ObjectInputStream(bais);
        HashMap<String,String> hm = (HashMap<String,String>)ois.readObject();
        ois.close();
        return hm.get("price");
    }

    private static String getBucketObject(String bucketkey) throws IOException {
        Regions region = Regions.US_EAST_1;
        AWSCredentials credentials = new BasicAWSCredentials(
                "AKIAT2YOH6JUCKMS37LU",
                "H8qx8ibv9lFPFqkHMmoqll+bU1TvVCVjAHD9zFx2"
        );
        String bucket = "19220456";
        String objKey = bucketkey;
        // get the serialize data from S3
        AmazonS3 s3Client = AmazonS3ClientBuilder
                .standard()
                .withCredentials(new AWSStaticCredentialsProvider(credentials))
                .withRegion(region)
                .build();

        S3Object s3o = s3Client.getObject(bucket,objKey);
        BufferedReader r = new BufferedReader(
                new InputStreamReader(s3o.getObjectContent(), Charset.forName("UTF-8"))
        );
        String line;
        String serialize = "";
        while ((line = r.readLine()) != null) {
            serialize += line;
        }
        return serialize;
    }
}
