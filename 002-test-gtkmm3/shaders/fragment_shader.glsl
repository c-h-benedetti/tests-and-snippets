#version 330 core

in vec3 v_color;
in vec3 v_pos;
in vec2 uv_pos;

out vec4 color;

uniform vec3 ptLoc;
uniform float threshold;

uniform sampler2D txt;
uniform sampler2D pts;

void main() {
    
    vec4 pt = texture(pts, uv_pos);
    vec4 tx = texture(txt, uv_pos);

    if (distance(v_pos, ptLoc) < threshold) {
        color = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        color = vec4(v_color, 1.0);
    }
    
    color *= tx;
    float alpha = pt.w;
    color = (1.0 - alpha) * color + alpha * pt;
}
