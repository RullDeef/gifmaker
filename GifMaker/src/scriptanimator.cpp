#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "duktape.h"
#include "scriptanimator.hpp"

ScriptAnimator::ScriptAnimator(const char *jsSource)
    : jsSource(jsSource)
{}

void ScriptAnimator::Render(float time)
{
    try
    {
        duk_get_global_string(ctx, "render");
        duk_push_number(ctx, time);
        if (duk_pcall(ctx, 1) != 0)
            std::cerr << "Error: " << duk_safe_to_string(ctx, -1);
        duk_pop(ctx);
    }
    catch (std::exception error)
    {
        std::cout << "Render error: " << error.what() << std::endl;
    }
}

// ================== Duktape bindings ===================

static ScriptAnimator *currInstance;

static duk_ret_t intr_print(duk_context *ctx)
{
    duk_push_string(ctx, " ");
    duk_insert(ctx, 0);
    duk_join(ctx, duk_get_top(ctx) - 1);
    printf("%s\n", duk_safe_to_string(ctx, -1));
    return 0;
}

// Renderer properties getters and setters

static duk_ret_t renderer_width_getter(duk_context *ctx)
{
    duk_push_int(ctx, currInstance->params.imageWidth);
    return 1;
}

static duk_ret_t renderer_width_setter(duk_context *ctx)
{
    if (duk_get_top(ctx) != 1 || !duk_check_type_mask(ctx, -1, DUK_TYPE_MASK_NUMBER))
    {
        std::cout << "Renderer.width is an integer property.\n";
        return DUK_RET_TYPE_ERROR;
    }

    currInstance->params.imageWidth = duk_opt_number(ctx, -1, currInstance->params.imageWidth);
    return 0;
}

static duk_ret_t renderer_height_getter(duk_context *ctx)
{
    duk_push_int(ctx, currInstance->params.imageHeight);
    return 1;
}

static duk_ret_t renderer_height_setter(duk_context *ctx)
{
    if (duk_get_top(ctx) != 1 || !duk_check_type_mask(ctx, -1, DUK_TYPE_MASK_NUMBER))
    {
        std::cout << "Renderer.height is an integer property.\n";
        return DUK_RET_TYPE_ERROR;
    }

    currInstance->params.imageHeight = duk_opt_number(ctx, -1, currInstance->params.imageHeight);
    return 0;
}

static duk_ret_t renderer_framesCount_getter(duk_context *ctx)
{
    duk_push_int(ctx, currInstance->params.framesCount);
    return 1;
}

static duk_ret_t renderer_framesCount_setter(duk_context *ctx)
{
    if (duk_get_top(ctx) != 1 || !duk_check_type_mask(ctx, -1, DUK_TYPE_MASK_NUMBER))
    {
        std::cout << "Renderer.framesCount is an integer property.\n";
        return DUK_RET_TYPE_ERROR;
    }

    currInstance->params.framesCount = duk_opt_number(ctx, -1, currInstance->params.framesCount);
    return 0;
}

static duk_ret_t renderer_frameRate_getter(duk_context *ctx)
{
    duk_push_number(ctx, currInstance->params.frameRate);
    return 1;
}

static duk_ret_t renderer_frameRate_setter(duk_context *ctx)
{
    if (duk_get_top(ctx) != 1 || !duk_check_type_mask(ctx, -1, DUK_TYPE_MASK_NUMBER))
    {
        std::cout << "Renderer.frameRate is a number property.\n";
        return DUK_RET_TYPE_ERROR;
    }

    currInstance->params.frameRate = duk_opt_number(ctx, -1, currInstance->params.frameRate);
    return 0;
}

static duk_ret_t renderer_blendType_getter(duk_context *ctx)
{
    duk_push_int(ctx, (int)currInstance->renderer.blendType);
    return 1;
}

static duk_ret_t renderer_blendType_setter(duk_context *ctx)
{
    if (duk_get_top(ctx) != 1 || !duk_check_type_mask(ctx, -1, DUK_TYPE_MASK_NUMBER))
    {
        std::cout << "Renderer.blendType is an integer property.\n";
        return DUK_RET_TYPE_ERROR;
    }

    currInstance->renderer.blendType = (Renderer::BlendType)duk_opt_int(ctx, -1, (int)currInstance->renderer.blendType);
    return 0;
}

static duk_ret_t renderer_prepTime_getter(duk_context *ctx)
{
    duk_push_number(ctx, currInstance->params.prepTime);
    return 1;
}

static duk_ret_t renderer_prepTime_setter(duk_context *ctx)
{
    if (duk_get_top(ctx) != 1 || !duk_check_type_mask(ctx, -1, DUK_TYPE_MASK_NUMBER))
    {
        std::cout << "Renderer.prepTime is a number property.\n";
        return DUK_RET_TYPE_ERROR;
    }

    currInstance->params.prepTime = duk_opt_number(ctx, -1, currInstance->params.prepTime);
    return 0;
}

static duk_ret_t renderer_lineWidth_getter(duk_context *ctx)
{
    duk_push_number(ctx, currInstance->renderer.lineWidth);
    return 1;
}

static duk_ret_t renderer_lineWidth_setter(duk_context *ctx)
{
    if (duk_get_top(ctx) != 1 || !duk_check_type_mask(ctx, -1, DUK_TYPE_MASK_NUMBER))
    {
        std::cout << "Renderer.lineWidth is an integer property.\n";
        return DUK_RET_TYPE_ERROR;
    }

    currInstance->renderer.lineWidth = duk_opt_number(ctx, -1, currInstance->renderer.lineWidth);
    return 0;
}

static duk_ret_t renderer_strokeColor_setter(duk_context *ctx)
{
    if (duk_get_top(ctx) != 1 || !duk_is_array(ctx, -1) || !(duk_get_length(ctx, -1) == 3 || duk_get_length(ctx, -1) == 4))
    {
        std::cout << "Renderer.strokeColor is an array of 3 or 4 numbers property.\n";
        return DUK_RET_TYPE_ERROR;
    }

    duk_get_prop_index(ctx, -1, 0);
    currInstance->renderer.strokeColor.r = duk_opt_number(ctx, -1, currInstance->renderer.strokeColor.r);
    duk_pop(ctx);

    duk_get_prop_index(ctx, -1, 1);
    currInstance->renderer.strokeColor.g = duk_opt_number(ctx, -1, currInstance->renderer.strokeColor.g);
    duk_pop(ctx);

    duk_get_prop_index(ctx, -1, 2);
    currInstance->renderer.strokeColor.b = duk_opt_number(ctx, -1, currInstance->renderer.strokeColor.b);
    duk_pop(ctx);

    if (duk_get_length(ctx, -1) == 4)
    {
        duk_get_prop_index(ctx, -1, 3);
        currInstance->renderer.strokeColor.a = duk_opt_number(ctx, -1, currInstance->renderer.strokeColor.a);
        duk_pop(ctx);
    }
    else
        currInstance->renderer.strokeColor.a = 255;

    return 0;
}

static duk_ret_t renderer_fillColor_setter(duk_context *ctx)
{
    if (duk_get_top(ctx) != 1 || !duk_is_array(ctx, -1) || !(duk_get_length(ctx, -1) == 3 || duk_get_length(ctx, -1) == 4))
    {
        std::cout << "Renderer.fillColor is an array of 3 or 4 numbers property.\n";
        return DUK_RET_TYPE_ERROR;
    }

    duk_get_prop_index(ctx, -1, 0);
    currInstance->renderer.fillColor.r = duk_opt_number(ctx, -1, currInstance->renderer.fillColor.r);
    duk_pop(ctx);

    duk_get_prop_index(ctx, -1, 1);
    currInstance->renderer.fillColor.g = duk_opt_number(ctx, -1, currInstance->renderer.fillColor.g);
    duk_pop(ctx);

    duk_get_prop_index(ctx, -1, 2);
    currInstance->renderer.fillColor.b = duk_opt_number(ctx, -1, currInstance->renderer.fillColor.b);
    duk_pop(ctx);

    if (duk_get_length(ctx, -1) == 4)
    {
        duk_get_prop_index(ctx, -1, 3);
        currInstance->renderer.fillColor.a = duk_opt_number(ctx, -1, currInstance->renderer.fillColor.a);
        duk_pop(ctx);
    }
    else
        currInstance->renderer.fillColor.a = 255;

    return 0;
}

// Renderer methods

static duk_ret_t renderer_Clear(duk_context *ctx)
{
    currInstance->renderer.Clear();
    return 0;
}

static duk_ret_t renderer_StrokeCircle(duk_context *ctx)
{
    try
    {
        if (duk_get_top(ctx) != 3 ||
            !duk_check_type(ctx, -1, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -2, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -3, DUK_TYPE_NUMBER))
        {
            std::cout << "Render.StrokeCircle(x, y, radius) - strokes circle at point x y.\n";
            return DUK_RET_TYPE_ERROR;
        }

        float x = duk_opt_number(ctx, -3, 0);
        float y = duk_opt_number(ctx, -2, 0);
        float radius = duk_opt_number(ctx, -1, 0);

        currInstance->renderer.StrokeCircle(x, y, radius);
    }
    catch (std::exception error)
    {
        std::cout << "exception: " << error.what() << std::endl;
    }

    return 0;
}

static duk_ret_t renderer_StrokeRect(duk_context *ctx)
{
    try
    {
        if (duk_get_top(ctx) != 4 ||
            !duk_check_type(ctx, -1, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -2, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -3, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -4, DUK_TYPE_NUMBER))
        {
            std::cout << "Render.StrokeRect(x, y, width, height) - strokes rect\n";
            return DUK_RET_TYPE_ERROR;
        }

        float x = duk_opt_number(ctx, -4, 0);
        float y = duk_opt_number(ctx, -3, 0);
        float width = duk_opt_number(ctx, -2, 0);
        float height = duk_opt_number(ctx, -1, 0);

        currInstance->renderer.StrokeRect(x, y, width, height);
    }
    catch (std::exception error)
    {
        std::cout << "exception: " << error.what() << std::endl;
    }

    return 0;
}

static duk_ret_t renderer_FillCircle(duk_context *ctx)
{
    try
    {
        if (duk_get_top(ctx) != 3 ||
            !duk_check_type(ctx, -1, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -2, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -3, DUK_TYPE_NUMBER))
        {
            std::cout << "Render.FillCircle(x, y, radius) - fills circle at point x y.\n";
            return DUK_RET_TYPE_ERROR;
        }

        float x = duk_opt_number(ctx, -3, 0);
        float y = duk_opt_number(ctx, -2, 0);
        float radius = duk_opt_number(ctx, -1, 0);

        currInstance->renderer.FillCircle(x, y, radius);
    }
    catch (std::exception error)
    {
        std::cout << "exception: " << error.what() << std::endl;
    }

    return 0;
}

static duk_ret_t renderer_FillRect(duk_context *ctx)
{
    try
    {
        if (duk_get_top(ctx) != 4 ||
            !duk_check_type(ctx, -1, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -2, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -3, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -4, DUK_TYPE_NUMBER))
        {
            std::cout << "Render.FillRect(x, y, width, height) - strokes rect\n";
            return DUK_RET_TYPE_ERROR;
        }

        float x = duk_opt_number(ctx, -4, 0);
        float y = duk_opt_number(ctx, -3, 0);
        float width = duk_opt_number(ctx, -2, 0);
        float height = duk_opt_number(ctx, -1, 0);

        currInstance->renderer.FillRect(x, y, width, height);
    }
    catch (std::exception error)
    {
        std::cout << "exception: " << error.what() << std::endl;
    }

    return 0;
}

static duk_ret_t renderer_BeginPath(duk_context *ctx)
{
    currInstance->renderer.BeginPath();
    return 0;
}

static duk_ret_t renderer_ClosePath(duk_context *ctx)
{
    currInstance->renderer.ClosePath();
    return 0;
}

static duk_ret_t renderer_Stroke(duk_context *ctx)
{
    currInstance->renderer.Stroke();
    return 0;
}

static duk_ret_t renderer_Fill(duk_context *ctx)
{
    currInstance->renderer.Fill();
    return 0;
}

static duk_ret_t renderer_MoveTo(duk_context *ctx)
{
    try
    {
        if (duk_get_top(ctx) != 2 ||
            !duk_check_type(ctx, -1, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -2, DUK_TYPE_NUMBER))
        {
            std::cout << "Render.MoveTo(x, y) - moves brush to given point\n";
            return DUK_RET_TYPE_ERROR;
        }

        float x = duk_opt_number(ctx, -2, 0);
        float y = duk_opt_number(ctx, -1, 0);

        currInstance->renderer.MoveTo({ x, y });
    }
    catch (std::exception error)
    {
        std::cout << "exception: " << error.what() << std::endl;
    }

    return 0;
}

static duk_ret_t renderer_LineTo(duk_context *ctx)
{
    try
    {
        if (duk_get_top(ctx) != 2 ||
            !duk_check_type(ctx, -1, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -2, DUK_TYPE_NUMBER))
        {
            std::cout << "Render.LineTo(x, y) - draws line to given point\n";
            return DUK_RET_TYPE_ERROR;
        }

        float x = duk_opt_number(ctx, -2, 0);
        float y = duk_opt_number(ctx, -1, 0);

        currInstance->renderer.LineTo({ x, y });
    }
    catch (std::exception error)
    {
        std::cout << "exception: " << error.what() << std::endl;
    }

    return 0;
}

static duk_ret_t renderer_Arc(duk_context *ctx)
{
    try
    {
        if (duk_get_top(ctx) < 5 || duk_get_top(ctx) > 6
            || (duk_get_top(ctx) == 5 && (
            !duk_check_type(ctx, -5, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -4, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -3, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -2, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -1, DUK_TYPE_NUMBER)))
            || (duk_get_top(ctx) == 6 && (
            !duk_check_type(ctx, -6, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -5, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -4, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -3, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -2, DUK_TYPE_NUMBER) ||
            !duk_check_type(ctx, -1, DUK_TYPE_BOOLEAN)))
            )
        {
            std::cout << "Render.Arc(x, y, radius, startAngle, endAngle[, clockwise=true]) - draws arc\n";
            return DUK_RET_TYPE_ERROR;
        }

        bool clockwisePushed = false;
        if (duk_get_top(ctx) != 6)
        {
            duk_push_true(ctx);
            clockwisePushed = true;
        }

        float x = duk_opt_number(ctx, -6, 0);
        float y = duk_opt_number(ctx, -5, 0);
        float radius = duk_opt_number(ctx, -4, 0);
        float startAnlge = duk_opt_number(ctx, -3, 0);
        float endAnlge = duk_opt_number(ctx, -2, 0);
        bool clockwise = duk_opt_boolean(ctx, -1, 0);

        currInstance->renderer.Arc({ x, y }, radius, startAnlge, endAnlge, clockwise);

        if (clockwisePushed)
            duk_pop(ctx);
    }
    catch (std::exception error)
    {
        std::cout << "exception: " << error.what() << std::endl;
    }

    return 0;
}

// =======================================================

void ScriptAnimator::PrepareDuktapeEnv()
{
    ctx = duk_create_heap_default();

    // internal just for debug function
    duk_push_c_function(ctx, intr_print, DUK_VARARGS);
    duk_put_global_string(ctx, "print");

    // put rendering API
    duk_push_global_object(ctx);
    duk_idx_t renderer_id = duk_get_top_index(ctx);

#define PUT_PROP(propName) \
    duk_push_string(ctx, #propName); \
    duk_push_c_function(ctx, renderer_ ## propName ## _getter, 0); \
    duk_push_c_function(ctx, renderer_ ## propName ## _setter, 1); \
    duk_def_prop(ctx, renderer_id, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER)

    PUT_PROP(width);
    PUT_PROP(height);
    PUT_PROP(prepTime);
    PUT_PROP(framesCount);
    PUT_PROP(frameRate);
    PUT_PROP(lineWidth);
    PUT_PROP(blendType);

#undef PUT_PROP

    duk_push_string(ctx, "strokeColor");
    duk_push_c_function(ctx, renderer_strokeColor_setter, 1);
    duk_def_prop(ctx, renderer_id, DUK_DEFPROP_HAVE_SETTER);

    duk_push_string(ctx, "fillColor");
    duk_push_c_function(ctx, renderer_fillColor_setter, 1);
    duk_def_prop(ctx, renderer_id, DUK_DEFPROP_HAVE_SETTER);

#define PUT_METHOD(name, nargs) \
    duk_push_c_function(ctx, renderer_ ## name, nargs); \
    duk_put_prop_string(ctx, renderer_id, # name)

    PUT_METHOD(Clear, 0);

    PUT_METHOD(StrokeCircle, 3);
    PUT_METHOD(StrokeRect, 4);
    PUT_METHOD(FillCircle, 3);
    PUT_METHOD(FillRect, 4);

    PUT_METHOD(BeginPath, 0);
    PUT_METHOD(ClosePath, 0);
    PUT_METHOD(Stroke, 0);
    PUT_METHOD(Fill, 0);

    PUT_METHOD(MoveTo, 2);
    PUT_METHOD(LineTo, 2);
    PUT_METHOD(Arc, DUK_VARARGS);

#undef PUT_METHOD

    // BlendType object

    duk_push_string(ctx, "BlendType");
    duk_idx_t blendType_id = duk_push_object(ctx);

#define ADD_BLEND_MODE(mode) \
    duk_push_string(ctx, # mode); \
    duk_push_int(ctx, (int) Renderer::BlendType::mode); \
    duk_def_prop(ctx, blendType_id, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE)

    ADD_BLEND_MODE(ADD);
    ADD_BLEND_MODE(MIX);
    ADD_BLEND_MODE(MULTIPLY);
    ADD_BLEND_MODE(OVERLAY);
    ADD_BLEND_MODE(SUBTRACT);

#undef ADD_BLEND_MODE

    duk_def_prop(ctx, renderer_id, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE);

    duk_put_global_string(ctx, "Renderer");

    duk_eval_string_noresult(ctx, jsSource);
}

void ScriptAnimator::Execute()
{
    currInstance = this;
    PrepareDuktapeEnv();

    // update renderer
    renderer.Resize(params.imageWidth, params.imageHeight);

    system("mkdir -p .super-temp-dir");

    float time = 0.0f;

    if (params.prepTime > 0)
    {
        Render(time);
        while (time < params.prepTime)
        {
            time += 1.0f / params.frameRate;
            Render(time);
        }
    }

    for (int i = 0; i < params.framesCount; i++)
    {
        time = params.prepTime + i / params.frameRate;
        Render(time);

        char tempFilename[32];
        snprintf(tempFilename, 32, ".super-temp-dir/%03i.png", i);
        renderer.Save(tempFilename);
    }

    char command[64];
    snprintf(command, 64, "convert -delay %.1f -loop 0 .super-temp-dir/*.png out.gif", 100.0f / params.frameRate);
    system(command);
    system("rm -rf .super-temp-dir");

    // realize duktape heap
    duk_destroy_heap(ctx);
}
