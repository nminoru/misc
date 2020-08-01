package jp.nminoru.jersey_jaxb_test.model;

import java.util.HashMap;
import java.util.Map;
import javax.xml.bind.annotation.adapters.XmlAdapter;


public class OptionsAdaptor extends XmlAdapter<Map<String, Object>, Options> {

    @Override
    public Map<String, Object> marshal(Options v) {
        System.out.println("# OptionsAdaptor.marshal = " + v);

        if (v != null) {
            Map<String, Object> map = new HashMap<>();
            map.put("key1", v.key1);
            return map;
        }

        return null;
    }

    @Override
    public Options unmarshal(Map<String, Object> v) {
        System.out.println("# OptionsAdaptor.unmarshal = " + v);

        Options options = new Options();

        if (v != null)
            options.key1 = v.get("key1");

        return options;
    }
}
