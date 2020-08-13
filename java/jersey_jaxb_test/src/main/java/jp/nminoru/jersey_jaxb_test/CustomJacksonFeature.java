package jp.nminoru.jersey_jaxb_test;

import javax.ws.rs.Consumes;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Configuration;
import javax.ws.rs.core.Feature;
import javax.ws.rs.core.FeatureContext;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.ext.MessageBodyReader;
import javax.ws.rs.ext.MessageBodyWriter;
import javax.ws.rs.ext.Provider;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;
import org.glassfish.jersey.CommonProperties;
import org.glassfish.jersey.internal.InternalProperties;
import org.glassfish.jersey.internal.util.PropertiesHelper;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.glassfish.jersey.jackson.internal.JacksonFilteringFeature;
import org.glassfish.jersey.jackson.internal.jackson.jaxrs.base.JsonMappingExceptionMapper;
import org.glassfish.jersey.jackson.internal.jackson.jaxrs.base.JsonParseExceptionMapper;
import org.glassfish.jersey.jackson.internal.jackson.jaxrs.json.JacksonJaxbJsonProvider;
import org.glassfish.jersey.message.filtering.EntityFilteringFeature;


/**
 * @see <a href="https://github.com/jersey/jersey/blob/master/media/json-jackson/src/main/java/org/glassfish/jersey/jackson/JacksonFeature.java">https://github.com/jersey/jersey/blob/master/media/json-jackson/src/main/java/org/glassfish/jersey/jackson/JacksonFeature.java</a>
 */
@Provider
@Produces(MediaType.APPLICATION_JSON)
public class CustomJacksonFeature extends JacksonFeature {

    private static final String JSON_FEATURE = JacksonFeature.class.getSimpleName();

    @Override
    public boolean configure(final FeatureContext context) {
        final Configuration config = context.getConfiguration();

        final String jsonFeature = CommonProperties.getValue(config.getProperties(), config.getRuntimeType(),
                InternalProperties.JSON_FEATURE, JSON_FEATURE, String.class);
        
        // Other JSON providers registered.
        if (!JSON_FEATURE.equalsIgnoreCase(jsonFeature)) {
            return false;
        }

        // Disable other JSON providers.
        context.property(PropertiesHelper.getPropertyNameForRuntime(InternalProperties.JSON_FEATURE, config.getRuntimeType()),
                JSON_FEATURE);

        // Register Jackson.
        if (!config.isRegistered(CustomJacksonJaxbJsonProvider.class)) {
            // add the default Jackson exception mappers
            context.register(JsonParseExceptionMapper.class);
            context.register(JsonMappingExceptionMapper.class);

            if (EntityFilteringFeature.enabled(config)) {
                throw new UnsupportedOperationException();
            } else {
                context.register(CustomJacksonJaxbJsonProvider.class, MessageBodyReader.class, MessageBodyWriter.class);
            }
        }

        return true;
    }

    @Provider
    @Consumes(MediaType.WILDCARD) // NOTE: required to support "non-standard" JSON variants
    @Produces(MediaType.WILDCARD)
    static class CustomJacksonJaxbJsonProvider extends JacksonJaxbJsonProvider {

        public CustomJacksonJaxbJsonProvider() {
            super();

            // シリアライズ: フィールドを持たないクラスをエラーとしない
            disable(SerializationFeature.FAIL_ON_EMPTY_BEANS);
            // デシリアライズ: クラスに対応するフィールドない JSON が来た時に無視する。
            disable(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES);
        }
    }
}
