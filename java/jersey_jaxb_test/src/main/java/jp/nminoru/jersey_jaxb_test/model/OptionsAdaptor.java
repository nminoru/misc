package jp.nminoru.jersey_jaxb_test.model;

import java.util.HashMap;
import java.util.Map;
import javax.xml.bind.annotation.adapters.XmlAdapter;


public class OptionsAdaptor extends XmlAdapter<Options, Map<String, Object>> {

    @Override
    public Options marshal(Map<String, Object> v) {
        System.out.println("# OptionsAdaptor.marshal = " + v);

        Options options = new Options();
        
        if (v != null)
            options.key1 = v.get("key1");

        return options;
    }

    @Override
    public Map<String, Object> unmarshal(Options v) {
        System.out.println("# OptionsAdaptor.unmarshal = " + v);

        if (v != null) {
            Map<String, Object> map = new HashMap<>();
            map.put("key1", v.key1);
            return map;
        }

        return null;
    }    
}
