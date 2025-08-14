import msgpack
import redis
import os

FLOW_CHAN = "mychannel"
DEFAULT_REDIS_HOST = "localhost"
DEFAULT_REDIS_PORT = "26379"

payload = [
    "python",
    {"id": 100, "comment": 'comment'}
]

pool = redis.ConnectionPool(host=DEFAULT_REDIS_HOST, port=DEFAULT_REDIS_PORT, db=0)
r = redis.StrictRedis(connection_pool=pool)
# r = redis.Redis(host='localhost', port='16397', db=0)
body = msgpack.packb(payload)
print(body)

print('----------------')

r.set(FLOW_CHAN, 'body')

print('----------------')

count = r.publish(FLOW_CHAN, body)
print(count)

r = redis.StrictRedis(connection_pool=pool)
result = r.get(FLOW_CHAN)
print(result)

