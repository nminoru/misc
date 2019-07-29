import java.io.IOException;
import java.util.List;
import javax.xml.namespace.QName;
import com.github.sardine.DavResource;
import com.github.sardine.Sardine;
import com.github.sardine.SardineFactory;


public class SarineTest {
    
    public static void main(String[] args) throws IOException {
        Sardine sardine1 = SardineFactory.begin("username", "password");

        doTest(sardine1, "/");

        Sardine sardine2 = SardineFactory.begin("username", "password");
        sardine2.list("http://127.0.0.1:8888/");
    }

    private static void doTest(Sardine sardine, String path) throws IOException {
        List<DavResource> resources = sardine.list("http://127.0.0.1:8888" + path);
        
        for (DavResource resource : resources) {
            System.out.println(resource);

            for (QName qname : resource.getResourceTypes()) {
                if (qname.getNamespaceURI().equals("DAV:") && qname.getLocalPart().equals("collection")) {
                    if (!path.equals(resource.getHref().getPath())) {
                        doTest(sardine, resource.getHref().getPath());
                    }
                }
            }
        }
        
    }
}
