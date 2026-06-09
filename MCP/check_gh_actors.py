import json
import socket

payload = json.dumps({"type": "get_actors_in_level", "params": {}}).encode("utf-8")
with socket.create_connection(("127.0.0.1", 55557), timeout=10) as sock:
    sock.sendall(payload)
    chunks = []
    sock.settimeout(10)
    try:
        while True:
            chunk = sock.recv(65536)
            if not chunk:
                break
            chunks.append(chunk)
            data = b"".join(chunks)
            try:
                obj = json.loads(data.decode("utf-8"))
                names = []
                actors = obj.get("result", {}).get("actors", [])
                for actor in actors:
                    name = actor.get("name", "")
                    if name.startswith("GH_"):
                        names.append(name)
                print(json.dumps({"gh_count": len(names), "gh_names": names[:100]}, ensure_ascii=False, indent=2))
                raise SystemExit
            except json.JSONDecodeError:
                pass
    except TimeoutError:
        print("timeout")
