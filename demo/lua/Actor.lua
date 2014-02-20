require "ymx"
require "Moveable"

Actor = Moveable:new()
function Actor:new(o)
	o.sprite = ymx.createSprite(o.component, o.texture)
	o.width = o.texture:getWidth() * 0.25
	o.height = o.texture:getHeight() * 0.25
	o.sprite:setTextureRect{0, o.direction * o.height, o.width, (o.direction + 1) * o.height}
	o.timeSinceSwitchAction = 0
	o.actionNum = 0
	setmetatable(o, self)
	self.__index = self
	return o
end

function Actor:move(delta)
	Moveable.move(self, delta)

	self.timeSinceSwitchAction = self.timeSinceSwitchAction + delta
	if self.timeSinceSwitchAction > 0.1 then
		self.timeSinceSwitchAction = 0
		self.actionNum = (self.actionNum + 1) % 4
		self.sprite:setTextureRect{
			self.actionNum * self.width,
			self.direction * self.height,
			self.actionNum * self.width + self.width,
			self.direction * self.height + self.height
		}
	end
end


function Actor:setDirection(direction)
	self.direction = direction
	self.sprite:setTextureRect{
		0,
		self.direction * self.height,
		self.width,
		self.direction * self.height + self.height
	}
end

function Actor:getBoundingBox()
	return {
		self.x + self.width * 0.1, 
		self.y + self.height * 0.1,
		self.x + self.width * 0.9,
		self.y + self.height * 0.9,
	}
end

return Actor

