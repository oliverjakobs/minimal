#include "Background.h"

#include "Graphics/Renderer.h"

static void BackgroundLayerUpdate(BackgroundLayer* layer, float x, float deltatime)
{
	float rel_dist = x * (1 - layer->parallax);

	layer->pos_x = layer->startpos + (x * layer->parallax);

	if (rel_dist > layer->startpos + layer->width) layer->startpos += layer->width;
	else if (rel_dist < layer->startpos - layer->width) layer->startpos -= layer->width;
}

static void BackgroundLayerRender(const BackgroundLayer* layer)
{
	BatchRenderer2DRenderTexture(&layer->texture, layer->pos_x - layer->width, layer->pos_y, layer->width, layer->height);
	BatchRenderer2DRenderTexture(&layer->texture, layer->pos_x, layer->pos_y, layer->width, layer->height);
	BatchRenderer2DRenderTexture(&layer->texture, layer->pos_x + layer->width, layer->pos_y, layer->width, layer->height);
}

int BackgroundAlloc(Background* background, size_t max_layers)
{
	background->layers = malloc(sizeof(BackgroundLayer) * max_layers);
	background->layer_count = 0;
	background->max_layers = max_layers;

	return background->layers != NULL;
}

void BackgroundFree(Background* background)
{
	for (size_t i = 0; i < background->layer_count; ++i)
		ignisDeleteTexture2D(&background->layers[i].texture);

	free(background->layers);
	background->layer_count = 0;
	background->max_layers = 0;
}

size_t BackgroundPushLayer(Background* background, const char* path, float x, float y, float w, float h, float parallax)
{
	if (background->layer_count >= background->max_layers) return 0;

	background->layers[background->layer_count] = (BackgroundLayer){
		.startpos = x,
		.pos_x = x,
		.pos_y = y,
		.width = w,
		.height = h,
		.parallax = parallax,
	};

	if (!ignisCreateTexture2D(&background->layers[background->layer_count].texture, path, 1, 1, 1, NULL))
		return 0;

	return background->layer_count++;
}

void BackgroundUpdate(Background* background, float x, float deltatime)
{
	for (size_t i = 0; i < background->layer_count; ++i)
		BackgroundLayerUpdate(&background->layers[i], x, deltatime);
}

void BackgroundRender(const Background* background, const float* mat_view_proj)
{
	BatchRenderer2DStart(mat_view_proj);

	for (size_t i = 0; i < background->layer_count; ++i)
		BackgroundLayerRender(&background->layers[i]);

	BatchRenderer2DFlush();
}
