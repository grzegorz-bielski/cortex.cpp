import { Inject, Injectable, NotFoundException } from '@nestjs/common';
import { CreateThreadDto } from '@/infrastructure/dtos/threads/create-thread.dto';
import { UpdateThreadDto } from '@/infrastructure/dtos/threads/update-thread.dto';
import { v4 as uuidv4 } from 'uuid';
import { PageDto } from '@/infrastructure/dtos/page.dto';
import { CreateMessageDto } from '@/infrastructure/dtos/threads/create-message.dto';
import { ulid } from 'ulid';
import { Message, MessageContent } from '@/domain/models/message.interface';
import { UpdateMessageDto } from '@/infrastructure/dtos/threads/update-message.dto';
import { Thread } from '@/domain/models/thread.interface';
import DeleteMessageDto from '@/infrastructure/dtos/threads/delete-message.dto';
import { Assistant } from '@/domain/models/assistant.interface';
import { Repository } from 'sequelize-typescript';
import { ThreadEntity } from '@/infrastructure/entities/thread.entity';
import { MessageEntity } from '@/infrastructure/entities/message.entity';
import { Op } from 'sequelize';

@Injectable()
export class ThreadsUsecases {
  constructor(
    @Inject('THREAD_REPOSITORY')
    private threadRepository: Repository<ThreadEntity>,
    @Inject('MESSAGE_REPOSITORY')
    private messageRepository:  Repository<MessageEntity>,
  ) {}

  async create(createThreadDto: CreateThreadDto): Promise<Thread> {
    const id = uuidv4();
    const { assistants } = createThreadDto;
    const assistantEntity: Assistant[] = assistants.map((assistant) => {
      const entity: Assistant = {
        ...assistant,
        response_format: null,
        tool_resources: null,
        top_p: assistant.top_p ?? null,
        temperature: assistant.temperature ?? null,
      };
      return entity;
    });

    const thread: Partial<Thread> = {
      id,
      assistants: assistantEntity,
      object: 'thread',
      created_at: Date.now(),
      title: 'New Thread',
      tool_resources: null,
      metadata: null,
    };

    return this.threadRepository.create(thread);
  }

  async findAll(): Promise<Thread[]> {
    return this.threadRepository.findAll({
      include: [{ all: true }],
      order: [['created_at', 'DESC']],
    });
  }

  async getMessagesOfThread(
    threadId: string,
    limit: number,
    order: 'asc' | 'desc',
    after?: string,
    before?: string,
    runId?: string,
  ) {
    await this.getThreadOrThrow(threadId);
    const normalizedOrder = order === 'asc' ? 'ASC' : 'DESC';

    const messages = await this.messageRepository.findAll({
      where: { thread_id: threadId },
      order: [['created_at', normalizedOrder]],
      limit: limit + 1,
      ...(after && { where: { id: { [Op.gt]: after } } }),
      ...(before && { where: { id: { [Op.lt]: before } } }),
    });

    let hasMore = false;
    if (messages.length > limit) {
      hasMore = true;
      messages.pop();
    }

    const firstId = messages[0]?.id ?? undefined;
    const lastId = messages[messages.length - 1]?.id ?? undefined;

    return new PageDto(messages, hasMore, firstId, lastId);
  }

  async createMessageInThread(
    threadId: string,
    createMessageDto: CreateMessageDto,
  ) {
    const thread = await this.getThreadOrThrow(threadId);
    const assistantId: string = thread.assistants[0].id;

    const messageContent: MessageContent = {
      type: 'text',
      text: {
        annotations: [],
        value: createMessageDto.content,
      },
    };

    const message: Partial<Message> = {
      id: ulid(),
      object: 'thread.message',
      thread_id: threadId,
      assistant_id: assistantId,
      created_at: Date.now(),
      status: 'completed',
      role: createMessageDto.role,
      content: [messageContent],
      metadata: null,
      run_id: null,
      completed_at: null,
      incomplete_details: null,
      attachments: [],
      incomplete_at: null,
    };

    await this.messageRepository.create(message);
    return message;
  }

  async updateMessage(
    threadId: string,
    messageId: string,
    updateMessageDto: UpdateMessageDto,
  ) {
    await this.getThreadOrThrow(threadId);
    await this.messageRepository.update(updateMessageDto, { where: { id: messageId } });
    return this.messageRepository.findOne({ where: { id: messageId } });
  }

  private async getThreadOrThrow(threadId: string): Promise<Thread> {
    const thread = await this.findOne(threadId);
    if (!thread) {
      throw new NotFoundException(`Thread with id ${threadId} not found`);
    }
    return thread;
  }

  private async getMessageOrThrow(messageId: string): Promise<Message> {
    const message = await this.messageRepository.findOne({
      where: { id: messageId },
    });
    if (!message) {
      throw new NotFoundException(`Message with id ${messageId} not found`);
    }
    return message;
  }

  findOne(id: string) {
    return this.threadRepository.findOne({ where: { id } });
  }

  async update(id: string, updateThreadDto: UpdateThreadDto) {
    const assistantEntities: Assistant[] =
      updateThreadDto.assistants?.map((assistant) => {
        const entity: Assistant = {
          ...assistant,
          name: assistant.name,
          response_format: null,
          tool_resources: null,
          top_p: assistant.top_p ?? null,
          temperature: assistant.temperature ?? null,
        };
        return entity;
      }) ?? [];

    const entity: Partial<Thread> = {
      ...updateThreadDto,
      assistants: assistantEntities,
    };

    return this.threadRepository.update(entity, { where: { id } });
  }

  async remove(id: string) {
    await this.threadRepository.destroy({ where: { id } });
  }

  async deleteMessage(_threadId: string, messageId: string): Promise<DeleteMessageDto> {
    await this.getMessageOrThrow(messageId);
    await this.messageRepository.destroy({ where: { id: messageId } });

    return {
      id: messageId,
      object: 'thread.message.deleted',
      deleted: true,
    };
  }

  async retrieveMessage(_threadId: string, messageId: string) {
    return this.getMessageOrThrow(messageId);
  }

  async clean(threadId: string) {
    await this.getThreadOrThrow(threadId);
    await this.messageRepository.destroy({ where: { thread_id: threadId } });
  }
}
