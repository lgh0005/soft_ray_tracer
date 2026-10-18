#include "pch.h"
#include "IRenderPipeline.h"

namespace bdt
{
	IRenderPipeline::IRenderPipeline(const String& tag) : m_tag(tag) { }
	IRenderPipeline::~IRenderPipeline() = default;
}
