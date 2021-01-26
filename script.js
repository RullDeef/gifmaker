Renderer.width = 200
Renderer.height = 200

Renderer.framesCount = 1
Renderer.blendType = Renderer.BlendType.ADD

function render(t) {
    Renderer.fillColor = [0, 0, 0, 0]
    Renderer.Clear()
    
    Renderer.fillColor = [255, 0, 0, 128]
    Renderer.FillCircle(75, 125, 40)
    
    Renderer.fillColor = [0, 255, 0, 128]
    Renderer.FillCircle(125, 125, 40)
    
    Renderer.fillColor = [0, 0, 255, 128]
    Renderer.FillCircle(100, 75, 40)
}
