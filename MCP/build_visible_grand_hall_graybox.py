import json
import socket
import time
from typing import Any, Dict, List

HOST = "127.0.0.1"
PORT = 55557
RUN_PREFIX = f"GH_{int(time.time())}"
CUBE = "/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube"
CYLINDER = "/Game/LevelPrototyping/Meshes/SM_Cylinder.SM_Cylinder"
CHAMFER = "/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube"


def send(command_type: str, params: Dict[str, Any]) -> Dict[str, Any] | None:
    payload = json.dumps({"type": command_type, "params": params}).encode("utf-8")
    with socket.create_connection((HOST, PORT), timeout=10) as sock:
        sock.sendall(payload)
        chunks: List[bytes] = []
        sock.settimeout(10)
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


def get_gh_names() -> List[str]:
    response = send("get_actors_in_level", {})
    actors = []
    if response:
        actors = response.get("result", {}).get("actors") or response.get("actors") or []
    return [actor.get("name", "") for actor in actors if actor.get("name", "").startswith("GH_")]


def delete_old_gh():
    for name in get_gh_names():
        send("delete_actor", {"name": name})


def actor_name(label: str) -> str:
    return f"{RUN_PREFIX}_{label}"


def spawn(label: str, loc, scale, rot=(0, 0, 0), actor_type="StaticMeshActor", mesh=CUBE):
    name = actor_name(label)
    params = {
        "name": name,
        "type": actor_type,
        "location": list(loc),
        "rotation": list(rot),
        "scale": list(scale),
    }
    if actor_type == "StaticMeshActor":
        params["static_mesh"] = mesh
    response = send("spawn_actor", params)
    if not response or response.get("status") != "success":
        print("spawn failed", name, response)
    return response


def build():
    delete_old_gh()

    spawn("001_MainHall_Floor", (0, 0, -20), (18, 12, 0.2))
    spawn("002_RearUpper_Platform", (620, 0, 260), (6.8, 11, 0.25))
    spawn("003_RearLower_Reception", (410, 0, 80), (2.8, 5, 0.25))
    spawn("010_BackWall", (900, 0, 330), (0.35, 12, 7))
    spawn("011_LeftWall", (250, -620, 230), (15, 0.3, 5.5))
    spawn("012_RightWall", (250, 620, 230), (15, 0.3, 5.5))
    spawn("013_UpperBack_Block", (820, 0, 620), (2.3, 7.5, 2.2))

    for side, y in [("Left", -390), ("Right", 390)]:
        sign = -1 if side == "Left" else 1
        for i in range(8):
            x = -150 + i * 72
            z = 8 + i * 28
            spawn(f"020_{side}Stair_Step_{i + 1:02d}", (x, y, z), (0.85, 2.5, 0.18))
        spawn(f"021_{side}Lower_Landing", (-220, y, 32), (2.3, 2.8, 0.28))
        spawn(f"022_{side}Upper_Landing", (500, y, 245), (2.8, 2.8, 0.28))
        spawn(f"023_{side}Outer_StairWall", (145, y + sign * 150, 120), (5.5, 0.18, 2.3))
        spawn(f"024_{side}Inner_StairRail", (215, y - sign * 145, 230), (4.2, 0.12, 0.75))

    spawn("030_CenterBalcony_Floor", (520, 0, 260), (2.9, 4.4, 0.2))
    spawn("031_CenterBalcony_Rail", (300, 0, 345), (0.18, 5.6, 0.55))
    spawn("032_LeftBalcony_Rail", (300, -315, 345), (0.18, 2.3, 0.55))
    spawn("033_RightBalcony_Rail", (300, 315, 345), (0.18, 2.3, 0.55))
    for i, y in enumerate([-450, -300, -150, 0, 150, 300, 450], start=1):
        spawn(f"034_BalconyRail_Post_{i:02d}", (282, y, 322), (0.12, 0.12, 0.65))

    column_positions = [
        (-260, -520), (-260, 520), (220, -515), (220, 515),
        (690, -280), (690, 280), (850, -520), (850, 520),
    ]
    for index, (x, y) in enumerate(column_positions, start=1):
        spawn(f"040_Column_{index:02d}", (x, y, 245), (0.45, 0.45, 5.3), mesh=CYLINDER)
        spawn(f"041_ColumnBase_{index:02d}", (x, y, 10), (0.85, 0.85, 0.25))
        spawn(f"042_ColumnCap_{index:02d}", (x, y, 500), (0.9, 0.9, 0.25))

    spawn("050_CenterDoor_LeftPillar", (880, -95, 240), (0.45, 0.35, 3.1))
    spawn("051_CenterDoor_RightPillar", (880, 95, 240), (0.45, 0.35, 3.1))
    spawn("052_CenterDoor_Header", (880, 0, 445), (0.45, 2.25, 0.45))
    spawn("053_ArchApprox_Top", (870, 0, 500), (0.42, 1.55, 0.22))
    spawn("054_RoundWindow_Block", (870, 0, 650), (0.18, 1.05, 1.05), mesh=CYLINDER)
    spawn("055_LeftWall_Niche", (400, -610, 250), (1.2, 0.12, 2.4))
    spawn("056_RightWall_Niche", (400, 610, 250), (1.2, 0.12, 2.4))

    spawn("060_CenterPiano_Block", (-250, 0, 45), (1.5, 1.1, 0.45), mesh=CHAMFER)
    spawn("061_Piano_LidHint", (-240, 0, 95), (1.25, 0.9, 0.12), rot=(0, 0, -8))
    spawn("062_PianoBench", (-420, 0, 25), (0.9, 0.32, 0.3))
    for side, y in [("Left", -520), ("Right", 520)]:
        spawn(f"063_{side}_ChairSeat", (-170, y, 25), (0.45, 0.45, 0.25))
        spawn(f"064_{side}_ChairBack", (-145, y, 80), (0.12, 0.45, 0.75))

    spawn("070_LeftWall_PointLight", (420, -570, 360), (1, 1, 1), actor_type="PointLight")
    spawn("071_RightWall_PointLight", (420, 570, 360), (1, 1, 1), actor_type="PointLight")
    spawn("072_CenterBalcony_PointLight", (540, 0, 455), (1, 1, 1), actor_type="PointLight")
    spawn("073_MainHall_SoftFill", (-250, 0, 420), (1, 1, 1), actor_type="PointLight")
    spawn("080_OverviewCamera", (-900, 0, 470), (1, 1, 1), rot=(-10, 0, 0), actor_type="CameraActor")
    send("focus_viewport", {"target": actor_name("060_CenterPiano_Block"), "distance": 1700})


if __name__ == "__main__":
    build()
    created = [name for name in get_gh_names() if name.startswith(RUN_PREFIX)]
    print(json.dumps({"prefix": RUN_PREFIX, "created_actor_count": len(created)}, ensure_ascii=False, indent=2))
