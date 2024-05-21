import codecs
import urllib.parse

word = 'user user#'

s =  codecs.encode(word.encode('utf-8'), 'hex_codec')
print(s)

urlencode = urllib.parse.quote(word);

print(urlencode)
print(urllib.parse.unquote(urlencode))





