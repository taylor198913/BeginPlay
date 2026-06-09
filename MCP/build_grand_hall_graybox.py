import json
import socket
from typing import Any, Dict, List

HOST = "127.0.0.1"
PORT = 55557


def send(command_type: str, params: Dict[str, Any]) -> Dict[str, Any] | None:
    payload = json.dumps({"type": command_type, "params": params}).encode("utf-8")
    with socket.create_connection((HOST, PORT), timeout=10) as sock:
        sock.sendall(payload)
        chunks: List[bytes] = []
        sock.settimeout(20)
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


def ok_or_exists(response: Dict[str, Any] | None) -> bool:
    if not response:
        return False
    if response.get("status") == "success":
        return True
    return "already exists" in response.get("error", "")


def ensure_mesh_blueprint(name: str, mesh_path: str):
    result = send("create_blueprint", {"name": name, "parent_class": "Actor"})
    if result and result.get("status") == "error" and "already exists" in result.get("error", ""):
        return
    if not ok_or_exists(result):
        print("create_blueprint failed", name, result)
        return

    component = send("add_component_to_blueprint", {
        "blueprint_name": name,
        "component_type": "StaticMeshComponent",
        "component_name": "Mesh",
        "location": [0, 0, 0],
        "rotation": [0, 0, 0],
        "scale": [1, 1, 1],
    })
    if component and component.get("status") == "error" and "already" in component.get("error", ""):
        pass

    mesh = send("set_static_mesh_properties", {
        "blueprint_name": name,
        "component_name": "Mesh",
        "static_mesh": mesh_path,
    })
    if mesh and mesh.get("status") == "error":
        print("set_static_mesh_properties failed", name, mesh)

    send("compile_blueprint", {"blueprint_name": name})


def delete_old_graybox():
    actors = send("get_actors_in_level", {})
    actor_list = []
    if actors:
        actor_list = actors.get("result", {}).get("actors") or actors.get("actors") or []
    for actor in actor_list:
        name = actor.get("name") or ""
        if name.startswith("GH_"):
            send("delete_actor", {"name": name})


def spawn(bp: str, name: str, loc, rot=(0, 0, 0), scale=(1, 1, 1)):
    result = send("spawn_blueprint_actor", {
        "blueprint_name": bp,
        "actor_name": name,
        "location": list(loc),
        "rotation": list(rot),
        "scale": list(scale),
    })
    if result and result.get("status") == "error" and "already exists" in result.get("error", ""):
        send("delete_actor", {"name": name})
        result = send("spawn_blueprint_actor", {
            "blueprint_name": bp,
            "actor_name": name,
            "location": list(loc),
            "rotation": list(rot),
            "scale": list(scale),
        })
    return result


def spawn_light(name: str, light_type: str, loc, rot=(0, 0, 0), scale=(1, 1, 1)):
    return send("spawn_actor", {
        "name": name,
        "type": light_type,
        "location": list(loc),
        "rotation": list(rot),
        "scale": list(scale),
    })


def build_scene():
    ensure_mesh_blueprint("BP_GH_Cube", "/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube")
    ensure_mesh_blueprint("BP_GH_Cylinder", "/Game/LevelPrototyping/Meshes/SM_Cylinder.SM_Cylinder")
    ensure_mesh_blueprint("BP_GH_Ramp", "/Game/LevelPrototyping/Meshes/SM_Ramp.SM_Ramp")
    ensure_mesh_blueprint("BP_GH_ChamferCube", "/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube")

    delete_old_graybox()

    # Main floor and rear elevated second-floor platform.
    spawn("BP_GH_Cube", "GH_001_MainHall_Floor", (0, 0, -20), scale=(18, 12, 0.2))
    spawn("BP_GH_Cube", "GH_002_RearUpper_Platform", (620, 0, 260), scale=(6.8, 11, 0.25))
    spawn("BP_GH_Cube", "GH_003_RearLower_Reception", (410, 0, 80), scale=(2.8, 5, 0.25))

    # Back wall, side walls, and upper facade mass.
    spawn("BP_GH_Cube", "GH_010_BackWall", (900, 0, 330), scale=(0.35, 12, 7))
    spawn("BP_GH_Cube", "GH_011_LeftWall", (250, -620, 230), scale=(15, 0.3, 5.5))
    spawn("BP_GH_Cube", "GH_012_RightWall", (250, 620, 230), scale=(15, 0.3, 5.5))
    spawn("BP_GH_Cube", "GH_013_UpperBack_Block", (820, 0, 620), scale=(2.3, 7.5, 2.2))

    # Two symmetric staircases, approximated by ramps plus side landings.
    spawn("BP_GH_Ramp", "GH_020_LeftGrandStair_Ramp", (180, -385, 95), rot=(0, 0, 0), scale=(4.6, 2.2, 1.9))
    spawn("BP_GH_Ramp", "GH_021_RightGrandStair_Ramp", (180, 385, 95), rot=(0, 180, 0), scale=(4.6, 2.2, 1.9))
    spawn("BP_GH_Cube", "GH_022_LeftLower_Landing", (-120, -385, 35), scale=(2.6, 2.8, 0.35))
    spawn("BP_GH_Cube", "GH_023_RightLower_Landing", (-120, 385, 35), scale=(2.6, 2.8, 0.35))
    spawn("BP_GH_Cube", "GH_024_LeftUpper_Landing", (520, -385, 260), scale=(2.7, 2.8, 0.25))
    spawn("BP_GH_Cube", "GH_025_RightUpper_Landing", (520, 385, 260), scale=(2.7, 2.8, 0.25))

    # Balcony rail line and center bridge, readable as the second-floor gallery in the image.
    spawn("BP_GH_Cube", "GH_030_CenterBalcony_Floor", (520, 0, 260), scale=(2.9, 4.4, 0.2))
    spawn("BP_GH_Cube", "GH_031_CenterBalcony_Rail", (300, 0, 345), scale=(0.18, 5.6, 0.55))
    spawn("BP_GH_Cube", "GH_032_LeftCurvedRail_Block", (290, -305, 345), scale=(0.18, 2.2, 0.55))
    spawn("BP_GH_Cube", "GH_033_RightCurvedRail_Block", (290, 305, 345), scale=(0.18, 2.2, 0.55))

    # Front and rear columns to sell the mansion lobby silhouette.
    column_positions = [
        (-260, -520), (-260, 520), (220, -515), (220, 515),
        (690, -280), (690, 280), (850, -520), (850, 520),
    ]
    for index, (x, y) in enumerate(column_positions, start=1):
        spawn("BP_GH_Cylinder", f"GH_040_Column_{index:02d}", (x, y, 245), scale=(0.55, 0.55, 5.3))
        spawn("BP_GH_Cube", f"GH_041_ColumnBase_{index:02d}", (x, y, 10), scale=(0.9, 0.9, 0.25))
        spawn("BP_GH_Cube", f"GH_042_ColumnCap_{index:02d}", (x, y, 500), scale=(0.95, 0.95, 0.25))

    # Central doorway/arch mass and side niches as simple graybox readable shapes.
    spawn("BP_GH_Cube", "GH_050_CenterDoor_LeftPillar", (880, -95, 240), scale=(0.45, 0.35, 3.1))
    spawn("BP_GH_Cube", "GH_051_CenterDoor_RightPillar", (880, 95, 240), scale=(0.45, 0.35, 3.1))
    spawn("BP_GH_Cube", "GH_052_CenterDoor_Header", (880, 0, 445), scale=(0.45, 2.25, 0.45))
    spawn("BP_GH_Cylinder", "GH_053_RoundWindow", (870, 0, 650), rot=(90, 0, 0), scale=(0.9, 0.9, 0.12))
    spawn("BP_GH_Cube", "GH_054_LeftWall_Niche", (400, -610, 250), scale=(1.2, 0.12, 2.4))
    spawn("BP_GH_Cube", "GH_055_RightWall_Niche", (400, 610, 250), scale=(1.2, 0.12, 2.4))

    # Central piano/table and a few chairs as scale references from the image.
    spawn("BP_GH_ChamferCube", "GH_060_CenterPiano_Block", (-250, 0, 40), scale=(1.5, 1.1, 0.45))
    spawn("BP_GH_Cube", "GH_061_PianoBench", (-420, 0, 25), scale=(0.9, 0.32, 0.3))
    for side, y in [("Left", -520), ("Right", 520)]:
        spawn("BP_GH_Cube", f"GH_062_{side}_ChairSeat", (-170, y, 25), scale=(0.45, 0.45, 0.25))
        spawn("BP_GH_Cube", f"GH_063_{side}_ChairBack", (-145, y, 80), scale=(0.12, 0.45, 0.75))

    # Wall lamps / atmosphere lights.
    spawn_light("GH_070_LeftWall_PointLight", "PointLight", (420, -570, 360))
    spawn_light("GH_071_RightWall_PointLight", "PointLight", (420, 570, 360))
    spawn_light("GH_072_CenterBalcony_PointLight", "PointLight", (540, 0, 455))
    spawn_light("GH_073_MainHall_SoftFill", "PointLight", (-250, 0, 420))

    # Camera overview for quick inspection.
    spawn_light("GH_080_OverviewCamera", "CameraActor", (-900, 0, 470), rot=(-10, 0, 0))
    send("focus_viewport", {"target": "GH_060_CenterPiano_Block", "distance": 1600})


if __name__ == "__main__":
    build_scene()
    print("Grand hall graybox generated with GH_ prefix actors.")
