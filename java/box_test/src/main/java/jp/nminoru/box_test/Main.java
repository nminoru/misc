package jp.nminoru.box_test;

import com.box.sdk.BoxAPIConnection;
import com.box.sdk.BoxFolder;
import com.box.sdk.BoxItem;
import com.box.sdk.BoxUser;


public class Main {
    public static final String BOX_TOKEN       = "BOX_TOKEN";
    public static final int    MAX_DEPTH       = 10;

    public static void main(String[] args) {
        String token = System.getenv(BOX_TOKEN);
        
        Main main = new Main();

        main.run(token);
    }
    
    void run(String token) {
        BoxAPIConnection api = new BoxAPIConnection(token);

        BoxUser.Info userInfo = BoxUser.getCurrentUser(api).getInfo();
        System.out.format("Welcome, %s <%s>!\n\n", userInfo.getName(), userInfo.getLogin());

        BoxFolder rootFolder = BoxFolder.getRootFolder(api);
        listFolder(rootFolder, 0);
    }

    void listFolder(BoxFolder folder, int depth) {
        for (BoxItem.Info itemInfo : folder) {
            StringBuilder indent = new StringBuilder();
            for (int i = 0; i < depth; i++) {
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
