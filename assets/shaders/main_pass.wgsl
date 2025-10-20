struct Frustum {
    top: vec4<f32>,
    bottom: vec4<f32>,
    right: vec4<f32>,
    left: vec4<f32>,
    far: vec4<f32>,
    near: vec4<f32>,
};

struct CameraBuffer {
    frustum: Frustum,
    view_matrix: mat4x4<f32>,
    projection_matrix: mat4x4<f32>,
    inv_view: mat4x4<f32>,
    inv_proj: mat4x4<f32>,
    view_pos: vec3<f32>,
    padding: f32,
    near: f32,
    far: f32,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3<f32>,
}

struct Vertex {
    position: vec3<f32>,
    uv_x: f32,
    normal: vec3<f32>,
    uv_y: f32,
}

struct Object {
    first_index: u32,
    first_vertex: u32,
}

struct Instance {
    matrix: mat4x4<f32>,
    velocity: vec3<f32>,
    object_index: u32,
    acceleration: vec3<f32>,
    _p0: u32,
    color: vec3<f32>,
    _p1: u32,
}

@group(0) @binding(0) var<uniform> u_camera: CameraBuffer;

@group(1) @binding(0) var<storage> objects: array<Object, 100>;

@group(1) @binding(2) var<storage> vertices: array<Vertex, 100000>;

@group(2) @binding(0) var<storage> instances: array<Instance, 200>;

@vertex
fn vertexMain(@builtin(vertex_index) v_id: u32, @builtin(instance_index) i_id: u32) -> VertexOutput {
    var out: VertexOutput;

    var instance = instances[i_id];
    var object = objects[instance.object_index];

    var vertex = vertices[v_id];

    out.position = u_camera.projection_matrix * u_camera.view_matrix * instances[i_id].matrix * vec4<f32>(vertex.position, 1.0);
    out.color = instance.color;
    return out;
}

@fragment
fn fragmentMain(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(in.color, 1.0);
}
