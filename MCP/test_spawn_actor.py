import json
import socket


def send(command_type, params):
    payload = json.dumps({"type": command_type, "params": params}).encode("utf-8")
    with socket.create_connection(("127.0.0.1", 55557), timeout=10) as sock:
        sock.sendall(payload)
        chunks = []
        sock.settimeout(15)
        while True:
            chunk = sock.recv(65536)
            if not chunk:
                break
            chunks.append(chunk)
            data = b"".join(chunks)
            try:
                return json.loads(data.decode("utf-8"))
            except json.JSONDecodeError:
                continue
    return json.loads(b"".join(chunks).decode("utf-8")) if chunks else None

for name in ["GH_TEST_Static", "GH_TEST_Light", "GH_TEST_Camera"]:
    send("delete_actor", {"name": name})

for command, params in [
    ("spawn_actor", {"name": "GH_TEST_Static", "type": "StaticMeshActor", "location": [0, 0, 100], "rotation": [0, 0, 0], "scale": [2, 2, 2]}),
    ("spawn_actor", {"name": "GH_TEST_Light", "type": "PointLight", "location": [0, 0, 400], "rotation": [0, 0, 0]}),
    ("spawn_actor", {"name": "GH_TEST_Camera", "type": "CameraActor", "location": [-800, 0, 400], "rotation": [-10, 0, 0]}),
]:
    print(command, json.dumps(send(command, params), ensure_ascii=False)[:1000])
