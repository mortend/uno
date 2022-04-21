using Uno;
using Uno.Diagnostics;
using OpenGL;

public class App : Application
{
    public App()
    {
        GL.ClearColor(1, 0, 0, 1);

        var vs = GL.CreateShader(GLShaderType.VertexShader);
        GL.ShaderSource(vs, VertexShaderSrc);
        Log.Debug(GL.GetShaderInfoLog(vs));

        //GL.DeleteShader(vs);

        Log.Debug("Success!?");
    }

    public override void Update()
    {
        Log.Debug("Update");
    }

    public override void Draw()
    {
        Log.Debug("Draw");
    }

    const string VertexShaderSrc =
        "#ifdef GL_ES\n" +
        "precision highp float;\n" +
        "#endif\n" +
        "uniform mat4 g, b;\n" +
        "uniform float c;\n" +
        "\n" +
        "attribute vec2 a;\n" +
        "\n" +
        "varying float h;\n" +
        "\n" +
        "vec2 i(vec2 j, float k){\n" +
        "    return vec2((j.x * cos(k)) - (j.y * sin(k)), (j.y * cos(k)) + (j.x * sin(k)));\n" +
        "}\n" +
        "\n" +
        "void main()\n" +
        "{\n" +
        "    h = i(a, c).y;\n" +
        "    gl_Position = b * (g * vec4(vec3(a, float(0)), 1.));\n" +
        "}\n";

    const string FragmentShaderSrc =
        "#ifdef GL_ES\n" +
        "precision highp float;\n" +
        "#endif\n" +
        "uniform float d;\n" +
        "uniform vec3 e, f;\n" +
        "\n" +
        "varying float h;\n" +
        "\n" +
        "void main()\n" +
        "{\n" +
        "    gl_FragColor = vec4(mix(e, f, (sin((h * 3.14159274) - d) * 0.5) + 0.5), float(1));\n" +
        "}\n";
}
