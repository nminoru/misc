package jp.nminoru.box_test;

import com.box.sdk.BoxAPIConnection;
import com.box.sdk.BoxCCGAPIConnection;
import com.box.sdk.BoxFolder;
import com.box.sdk.BoxItem;
import com.box.sdk.BoxUser;


public class Main {
    public static final String BOX_CLIENT_ID     = "BOX_CLIENT_ID";
    public static final String BOX_CLIENT_SECRET = "BOX_CLIENT_SECRET";
    public static final String BOX_USER_ID       = "BOX_USER_ID";
    public static final int    MAX_DEPTH         = 10;

    public static void main(String[] args) {
        String clientId     = System.getenv(BOX_CLIENT_ID);
        String clientSecret = System.getenv(BOX_CLIENT_SECRET);
        String userId       = System.getenv(BOX_USER_ID);
        
        Main main = new Main();

        System.out.println("BOX_CLIENT_ID    : " + clientId);
        System.out.println("BOX_CLIENT_SECRET: " + clientSecret);
        System.out.println("BOX_USER_ID      : " + userId);

        main.run(clientId, clientSecret, userId);
    }
    
    void run(String clientId, String clientSecret, String userId) {
        // BoxAPIConnection api = BoxCCGAPIConnection.applicationServiceAccountConnection(clientId, clientSecret, userId);
        BoxAPIConnection api = BoxCCGAPIConnection.userConnection(clientId, clientSecret, userId);

        BoxUser.Info userInfo = BoxUser.getCurrentUser(api).getInfo();
        System.out.format("Welcome, %s <%s>!\n\n", userInfo.getName(), userInfo.getLogin());

        BoxFolder rootFolder = BoxFolder.getRootFolder(api);
        listFolder(rootFolder, 0);
    }

    void listFolder(BoxFolder folder, int depth) {
        for (BoxItem.Info itemInfo : folder) {
            StringBuilder indent = new StringBuilder();

            indent.append(itemInfo.getID());
            
            for (int i = 0; i < depth + 1; i++) {
                indent.append("    ");
            }

            System.out.println(indent + itemInfo.getName());
            if (itemInfo instanceof BoxFolder.Info) {
                BoxFolder childFolder = (BoxFolder) itemInfo.getResource();
                if (depth < MAX_DEPTH) {
                    listFolder(childFolder, depth + 1);
                }
            }
        }
    }
}
