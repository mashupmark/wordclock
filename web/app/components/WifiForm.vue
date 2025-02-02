<script lang="ts" setup>
import * as v from "valibot";
import type { FormSubmitEvent } from "#ui/types";
import SettingsSection from "./SettingsSection.vue";

const toast = useToast();

const schema = v.object({
  SSID: v.pipe(v.string(), v.trim(), v.nonEmpty("required")),
  PW: v.pipe(v.string(), v.trim(), v.nonEmpty("required")),
});
type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({ SSID: "", PW: "" });

const onSubmit = async (data: FormSubmitEvent<Schema>) => {
  try {
    await $fetch("/api/settings/wifi", { method: "PUT", body: data });
  } catch {
    toast.add({
      title: "Error",
      description: "Failed to update wifi credentials",
      color: "red",
    });
  }
};
</script>

<template>
  <SettingsSection
    title="Wifi"
    description="Update the wifi credentials used to connect to the internet"
    :schema
    :state
    @submit="onSubmit"
  >
    <UFormGroup label="SSID" name="SSID" required>
      <UInput v-model="state.SSID" />
    </UFormGroup>
    <UFormGroup label="Password" name="PW" required>
      <UInput
        v-model="state.PW"
        type="password"
        trailing-icon="i-heroicons-lock-closed"
      />
    </UFormGroup>
  </SettingsSection>
</template>
