const W = 200
const H = 200

const leafs = 6
const amount = 10

const ball_radius = Math.min(W, H) / 4

Renderer.width = W
Renderer.height = H

Renderer.frameRate = 40
Renderer.framesCount = 2 * Math.PI * Renderer.frameRate
Renderer.prepTime = 60 // 2 * Math.PI * 3
Renderer.lineWidth = 1

const dt = 1 / Renderer.frameRate

function hsl(h, s, l) {
    var r, g, b

    if (s == 0) {
        r = g = b = l; // achromatic
    } else {
        var hue2rgb = function hue2rgb(p, q, t) {
            if (t < 0) t += 1;
            if (t > 1) t -= 1;
            if (t < 1 / 6) return p + (q - p) * 6 * t;
            if (t < 1 / 2) return q;
            if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
            return p;
        }

        var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        var p = 2 * l - q;
        r = hue2rgb(p, q, h + 1 / 3);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1 / 3);
    }

    return [Math.round(r * 255), Math.round(g * 255), Math.round(b * 255)];
}

function render(t) {
    const t_1 = t
    const t_2 = t + dt

    const ball_x_1 = W / 2 + ball_radius * Math.cos(t_1);
    const ball_y_1 = H / 2 + ball_radius * Math.sin(t_1);
    const ball_x_2 = W / 2 + ball_radius * Math.cos(t_2);
    const ball_y_2 = H / 2 + ball_radius * Math.sin(t_2);

    if (t == 0) {
        Renderer.fillColor = [0, 0, 0]
        Renderer.Clear()
    } else {
        Renderer.fillColor = [0, 0, 0, 4]
        Renderer.FillRect(0, 0, W, H)
    }

    Renderer.strokeColor = hsl(t * 3 / Math.PI - Math.floor(t * 3 / Math.PI), 0.5, 0.5);

    for (var i = 0; i < amount; i++) {
        const a_1 = 3 * t_1 + 2 * Math.PI * i / amount + Math.PI * Math.sin(t_1 / 2);
        const R_1 = 2 / 3 * ball_radius * (1 + Math.sin((leafs - 1) * t_1 + a_1) / 2);

        const a_2 = 3 * t_2 + 2 * Math.PI * i / amount + Math.PI * Math.sin(t_2 / 2);
        const R_2 = 2 / 3 * ball_radius * (1 + Math.sin((leafs - 1) * t_2 + a_2) / 2);

        const x_1 = ball_x_1 + R_1 * Math.cos(a_1);
        const y_1 = ball_y_1 + R_1 * Math.sin(a_1);

        const x_2 = ball_x_2 + R_2 * Math.cos(a_2);
        const y_2 = ball_y_2 + R_2 * Math.sin(a_2);

        Renderer.BeginPath();
        Renderer.MoveTo(x_1, y_1);
        Renderer.LineTo(x_2, y_2);
        Renderer.Stroke();
    }
}
