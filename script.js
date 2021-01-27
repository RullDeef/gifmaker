const W = 200
const H = 200

Renderer.width = W
Renderer.height = H

Renderer.frameRate = 60
Renderer.framesCount = 1
Renderer.lineWidth = 4

function render(t) {
    Renderer.fillColor = [255, 255, 255]
    Renderer.Clear()
    
    Renderer.BeginPath()
    Renderer.MoveTo(20, 50)
    Renderer.LineTo(120, 20)
    Renderer.Arc(150, 150, 40, -Math.PI / 2, Math.PI)
    Renderer.ClosePath()
    
    Renderer.fillColor = [192, 192, 64]
    Renderer.Fill()
    Renderer.Stroke()
}
