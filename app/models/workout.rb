class Workout < ActiveRecord::Base
  def to_csv
    self.class.columns.collect do |c|
      "#{c.name},#{c.type},#{self.send(c.name)}"
    end.join("\n")
  end
end
