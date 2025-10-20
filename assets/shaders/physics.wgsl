struct Instance {
    matrix: mat4x4<f32>,
    velocity: vec3<f32>,
    object_index: u32,
    acceleration: vec3<f32>,
    _p0: u32,
    color: vec3<f32>,
    _p1: u32,
}

struct SceneInfo {
    object_count: u32,
    instance_count: u32,
    index_count: u32
};

struct AABB {
    min: vec3<f32>,
    _p0: f32,
    max: vec3<f32>,
};

@group(0) @binding(0) var<storage, read_write> instances: array<Instance>;

@group(1) @binding(1) var<storage, read_write> aabbs: array<AABB>;

@group(1) @binding(4) var<uniform> scene_info: SceneInfo;

@group(2) @binding(0) var<uniform> u_dt: f32;


fn AABBCheck(a_min: vec4<f32>, a_max: vec4<f32>, b_min: vec4<f32>, b_max: vec4<f32>) -> bool {
    let x_check = a_min.x <= b_max.x && a_max.x >= b_min.x;
    let y_check = a_min.y <= b_max.y && a_max.y >= b_min.y;
    let z_check = a_min.z <= b_max.z && a_max.z >= b_min.z;
    return x_check && y_check && z_check;
}

fn MoveMatrix(old: mat4x4<f32>, delta: vec3<f32>) -> mat4x4<f32> {
    return mat4x4<f32>(
        old[0],
        old[1],
        old[2],
        vec4<f32>(old[3].x + delta.x, old[3].y + delta.y, old[3].z + delta.z, 1.0)
    );
}

@compute @workgroup_size(32)
fn main(@builtin(global_invocation_id) g_work_id: vec3<u32>) {
    let instance = instances[g_work_id.x];
    let aabb = aabbs[instance.object_index];
    let aabb_min = instance.matrix * vec4<f32>(aabb.min, 1.0f);
    let aabb_max = instance.matrix * vec4<f32>(aabb.max, 1.0f);

    var collision = false;

    for (var i: u32 = 0u; i < scene_info.instance_count; i = i + 1u) {
        if i == g_work_id.x {
          continue;
        }
        let other_instance = instances[i];
        let other_aabb = aabbs[other_instance.object_index] ;
        let other_aabb_min = other_instance.matrix * vec4<f32>(other_aabb.min, 1.0f);
        let other_aabb_max = other_instance.matrix * vec4<f32>(other_aabb.max, 1.0f);
        collision = AABBCheck(aabb_min, aabb_max, other_aabb_min, other_aabb_max);
    }

    if !collision {
        instances[g_work_id.x].velocity += instances[g_work_id.x].acceleration * u_dt;
        instances[g_work_id.x].matrix = MoveMatrix(instances[g_work_id.x].matrix, instances[g_work_id.x].velocity * u_dt);
    }
}
