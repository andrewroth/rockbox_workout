module Csv
  def to_csv
    self.class.columns.collect { |c|
      out = "#{c.name},#{c.type},#{self.send(c.name)}"

      if c.type == :datetime
        out += "\n#{c.name}_int,#{c.type},#{self.send(c.name).to_i}"
      end

      out
    }.join("\n")
  end
  def self.included(base)
    base.class_eval do
      def self.write_csv(path = nil)
        path ||= "transfer/#{self.name.underscore.pluralize}.csv"
        f = File.open(path, "w")
        if block_given?
          rows = yield
        else
          rows = all
        end
        f.write rows.collect(&:to_csv).join("\n") + "\n"
        f.close
      end
    end
  end
end

class Array
  def to_csv
    self.collect { |row|
      row.to_csv
    }.join("\n")
  end
end
