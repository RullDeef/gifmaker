const W = 400
const H = 400

Renderer.width = W
Renderer.height = H

Renderer.frameRate = 60
Renderer.framesCount = 60

/* state = [0...1] */
function packman(x, y, r, state) {
    Renderer.fillColor = [192, 192, 64]

    const angle = Math.PI / 3 * state

    if (state != 0) {
        Renderer.BeginPath()
        Renderer.MoveTo(x, y)
        Renderer.Arc(x, y, r, angle, -angle)
        Renderer.ClosePath()
        Renderer.Fill()
    }
    else
        Renderer.FillCircle(x, y, r)
}

function bump(t) {
    return Math.abs(2 * (t - 0.5))
}

function render(t) {
    Renderer.fillColor = [0, 0, 0]
    Renderer.Clear()

    const x = W * 3 / 9
    const y = H / 2
    const radius = W * 2 / 9
    const state = 1 - bump(bump(t))
    const dotState = t - 0.25

    packman(x, y, radius, state)

    for (var i = 0; i < 3; i++)
        Renderer.FillCircle(1.4 * W - i * W / 3 - dotState * W / 3, y, 0.1 * radius)
}
