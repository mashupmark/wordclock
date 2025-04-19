export type ClockSettings = {
  animation: {
    // The extra property is neccessary as ConfigDB expects it for tagging the oneof union
    Static: { name: "static"; color: string };
  };
};

export type GeneralSettings = {
  brightness: number;
  timezone: string;
};
