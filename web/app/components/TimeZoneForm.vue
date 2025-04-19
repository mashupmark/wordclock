<script lang="ts" setup>
import * as v from "valibot";
import SettingsSection from "./SettingsSection.vue";
import { timeZonesNames } from "@vvo/tzdb";

const toast = useToast();

const schema = v.object({ timezone: v.string() });
type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({ timezone: "" });

const { status, data } = useFetch<{ timezone: string }>("/api/settings", {
  server: false,
});
watchEffect(() => {
  if (!data.value) return;
  state.timezone = data.value.timezone;
});

const { status: saveStatus, execute: save } = useAsyncData(
  async () => {
    try {
      await $fetch("/api/settings", {
        method: "POST",
        body: { timezone: state.timezone },
      });
    } catch {
      toast.add({ title: "Failed to update time zone", color: "red" });
    }
  },
  { immediate: false }
);
</script>

<template>
  <SettingsSection
    title="Time zone"
    description="Set the time zone the time should be displayed in"
    :schema
    :state
    :isSaving="saveStatus === 'pending'"
    @submit="save()"
  >
    <UFormGroup label="Time zone" name="timezone" required>
      <UInputMenu
        v-model="state.timezone"
        :options="timeZonesNames"
        :loading="status === 'pending'"
      />
    </UFormGroup>
  </SettingsSection>
</template>
