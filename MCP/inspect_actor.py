import json
import socket
payload=json.dumps({"type":"get_actor_properties","params":{"name":"GH_TEST_Static"}}).encode()
with socket.create_connection(("127.0.0.1",55557),timeout=10) as s:
    s.sendall(payload)
    data=b""
    s.settimeout(10)
    while True:
        c=s.recv(65536)
        if not c: break
        data+=c
        try:
            print(json.dumps(json.loads(data.decode()),ensure_ascii=False,indent=2)[:4000])
            break
        except Exception: pass
