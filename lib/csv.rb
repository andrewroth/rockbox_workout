module Csv
  def to_csv
    self.class.columns.collect { |c|
      "#{c.name},#{c.type},#{self.send(c.name)}"
    }.join("\n")
  end
end

class Array
  def to_csv
    self.collect { |row|
      puts row.inspect
      row.to_csv
    }.join("\n")
  end
end
