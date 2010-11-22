class Exercise < ActiveRecord::Base
  include Csv

  belongs_to :exercise_type

  def name_with_type
    "#{name} (#{exercise_type.try(:name)})"
  end
end
