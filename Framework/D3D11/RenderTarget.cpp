#include "Framework.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(Context * context)
{
	m_graphics = context->GetSubsystem<Graphics>();
}

void RenderTarget::Create(const UINT width, const UINT height, const DXGI_FORMAT & pixel, const DXGI_FORMAT & depth)
{
}

void RenderTarget::BindPipeline()
{
}
