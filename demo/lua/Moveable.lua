require "ymx"

module(..., package.seeall)

Moveable.DOWN = 0
Moveable.LEFT = 1
Moveable.RIGHT = 2
Moveable.UP = 3

function Moveable:new(o)
	o = o or {}
	if o.texture then
		o.sprite = ymx.createSprite(o.component, o.texture)
		o.width = o.sprite:getWidth()
		o.height = o.sprite:getHeight()
	end
	setmetatable(o, self)
	self.__index = self
	return o
end

function Moveable:isCollide(o)
	return ymx.intersect(self:getBoundingBox(), o:getBoundingBox())
end

function Moveable:getBoundingBox()
	return {
		self.x, self.y, self.x + self.width, self.y + self.height
	}
end

function Moveable:render(g)
	self.sprite:render(g, self.x, self.y)
end

function Moveable:move(delta)
	if self.direction == Moveable.LEFT then
		self.x = self.x - self.velocity.x * delta
	elseif self.direction == Moveable.RIGHT then
		self.x = self.x + self.velocity.x * delta
	elseif self.direction == Moveable.UP then
		self.y = self.y - self.velocity.y * delta;
	else 
		self.y = self.y + self.velocity.y * delta;
	end
end

function Moveable:release()
	self.sprite:release()
end
