#include "TextureAtlases.h"

#include "imgui.h"


SubTexture& SubTexture::Create(glm::vec2 position, glm::vec2 nomalizeSize, const glm::vec2& imageSize)
{
	SubTexture* subTexture = new SubTexture;
	subTexture->imageCoord[0] = { position.x + nomalizeSize.x		, position.y };
	subTexture->imageCoord[1] = { position.x						, position.y };
	subTexture->imageCoord[2] = { position.x + nomalizeSize.x		, position.y + nomalizeSize.y };
	subTexture->imageCoord[3] = { position.x						, position.y + nomalizeSize.y };

	subTexture->size = nomalizeSize * imageSize;
	
	return *subTexture;
}



TextureAtlases::TextureAtlases(Texture* texture, glm::vec2 size)
	:m_Texture(texture), m_Size(size)
{
	m_ImageSize.x = m_Texture->GetWidth();
	m_ImageSize.y = m_Texture->GetHeigth();
	m_EndPosition.x = m_ImageSize.x;
	m_EndPosition.y = m_ImageSize.y;
	ResetPosition();
}

void TextureAtlases::ResetPosition()
{
	m_Position.x = m_StartPosition.x;
	m_Position.y = m_ImageSize.y - m_StartPosition.y - m_Size.y;
}

void TextureAtlases::ChangeStartPosition(float x, float y)
{
	m_StartPosition = { x * m_Size.x, y * m_Size.y };
}

void TextureAtlases::ChangeEndPosition(float x, float y)
{
	if (x == -1)
		m_EndPosition.x = m_ImageSize.x;
	else
		m_EndPosition.x = x * m_Size.x;
	if (y == -1)
		m_EndPosition.y = m_ImageSize.y;
	else
		m_EndPosition.y = y * m_Size.y;
}

void TextureAtlases::SetSubTextureSize(uint32_t w, uint32_t h)
{
	m_Size.x = w;
	m_Size.y = h;
}

void TextureAtlases::Push(uint32_t count, uint8_t mulW, uint8_t mulH)
{
	//SubTexture& subTexture = SubTexture::Create({ 16.0f / 160 * 0, 16.0f / 160 * 9 }, { 16.0f / 160, 16.0f / 160 });
	if(count <= 0)
		return;
	subTextures.push_back(SubTexture::Create(m_Position / m_ImageSize, m_Size / m_ImageSize * glm::vec2{ mulH, mulW }, m_ImageSize));

	Skip();

	Push(--count, mulW, mulH);
}

void TextureAtlases::Skip(int32_t count)
{
	if (count <= 0)
		return;

	m_Position.x += m_Size.x;

	if (m_Position.x >= m_EndPosition.x)
	{
		SkipLine();
	}
	Skip(--count);
}

void TextureAtlases::SkipLine(int32_t count)
{
	if (count <= 0)
		return;

	m_Position.x = m_StartPosition.x;
	m_Position.y -= m_Size.y;
	SkipLine(--count);
}

size_t TextureAtlases::Size() const
{
	return subTextures.size();
}



void TextureAtlasesGui::RenderImGui()
{
	ImGui::Begin("Object");
	ImVec2 wsize = { 35, 35 };
	
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(283.0f / 360.0f, 52.0f / 100.0f, 54.0f / 100.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(272.0f / 360.0f, 60.0f / 100.0f, 50.0f / 100.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(272.0f / 360.0f, 60.0f / 100.0f, 50.0f / 100.0f));
	if(m_SelectedItem == -1)
	{
		ImGuiIO& io = ImGui::GetIO();
		static ImVec2 uv0(0.34f, 0.0f);
		static ImVec2 uv1(0.365f, 0.23f);
		if (ImGui::ImageButton(io.Fonts->TexID, wsize, uv0, uv1, 0));
			
	}
	else
	{
		if (ImGui::ImageButton((ImTextureID)m_TextureAtlases.GetTextureId(), wsize,
			ImVec2(m_TextureAtlases.subTextures[m_SelectedItem].imageCoord[3].x, m_TextureAtlases.subTextures[m_SelectedItem].imageCoord[3].y),
			ImVec2(m_TextureAtlases.subTextures[m_SelectedItem].imageCoord[0].x, m_TextureAtlases.subTextures[m_SelectedItem].imageCoord[0].y),
			0
		))
			m_SelectedItem = -1;
	}
	ImGui::PopStyleColor(3);

	for (int i = 0; i < m_TextureAtlases.Size(); i++)
	{
		ImGui::PushID(i);
		int frame_padding = -1 + i;
		ImVec2 wsize = { 35, 35 };

		//ImGui::Image((ImTextureID)textureAtlases.GetTextureId(), wsize,
		//	ImVec2(textureAtlases.subTextures[0].imageCoord[3].x, textureAtlases.subTextures[0].imageCoord[3].y),
		//	ImVec2(textureAtlases.subTextures[0].imageCoord[0].x, textureAtlases.subTextures[0].imageCoord[0].y));
		if(m_SelectedItem == i)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(283.0f / 360.0f, 52.0f / 100.0f, 54.0f / 100.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(272.0f / 360.0f, 60.0f / 100.0f, 50.0f / 100.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(272.0f / 360.0f, 60.0f / 100.0f, 50.0f / 100.0f));
			if (ImGui::ImageButton((ImTextureID)m_TextureAtlases.GetTextureId(), wsize,
				ImVec2(m_TextureAtlases.subTextures[i].imageCoord[3].x, m_TextureAtlases.subTextures[i].imageCoord[3].y),
				ImVec2(m_TextureAtlases.subTextures[i].imageCoord[0].x, m_TextureAtlases.subTextures[i].imageCoord[0].y),
				2
			))
				m_SelectedItem = 0;
			ImGui::PopStyleColor(3);
		}
		else
		{
			if (ImGui::ImageButton((ImTextureID)m_TextureAtlases.GetTextureId(), wsize,
				ImVec2(m_TextureAtlases.subTextures[i].imageCoord[3].x, m_TextureAtlases.subTextures[i].imageCoord[3].y),
				ImVec2(m_TextureAtlases.subTextures[i].imageCoord[0].x, m_TextureAtlases.subTextures[i].imageCoord[0].y),
				1
			))
				m_SelectedItem = i;
		}
		ImGui::PopID();

		if (!((ImGui::GetWindowSize().x + ImGui::GetWindowPos().x) < ImGui::GetItemRectMax().x + ImGui::GetItemRectSize().x))
			ImGui::SameLine();
	}
	ImGui::NewLine();
	ImGui::Text("Selected Item: %d.", m_SelectedItem);
	ImGui::End();
}
