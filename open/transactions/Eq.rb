module Eq
	def == (o)
		return false unless self.class == o.class
		instance_variables.each do |v|
			return false unless instance_variable_get(v) == o.instance_variable_get(v)
		end
		return true
	end
end
